/* $Id: exo-uri.h,v 1.2 2004/09/17 09:48:24 bmeurer Exp $ */
/*-
 * Copyright (c) 2004  Benedikt Meurer <benny@xfce.org>
 * Copyright (c) 2003  Marco Pesenti Gritti
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <stdio.h>

#include <libxml/tree.h>

#include <exo/exo-marshal.h>
#include <exo/exo-string.h>
#include <exo/exo-toolbars-model.h>
#include <exo/exo-toolbars-private.h>



#define EXO_TOOLBARS_MODEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_TOOLBARS_MODEL, ExoToolbarsModelPrivate))



typedef struct _ExoToolbarsToolbar ExoToolbarsToolbar;
typedef struct _ExoToolbarsItem    ExoToolbarsItem;



enum
{
  ITEM_ADDED,
  ITEM_REMOVED,
  TOOLBAR_ADDED,
  TOOLBAR_CHANGED,
  TOOLBAR_REMOVED,
  GET_ITEM_TYPE,
  GET_ITEM_ID,
  GET_ITEM_DATA,
  LAST_SIGNAL,
};



static void             exo_toolbars_model_class_init           (ExoToolbarsModelClass  *klass);
static void             exo_toolbars_model_init                 (ExoToolbarsModel       *model);
static void             exo_toolbars_model_finalize             (GObject                *object);
static gboolean         exo_toolbars_model_real_add_item        (ExoToolbarsModel       *model,
                                                                 gint                    toolbar_position,
                                                                 gint                    item_position,
                                                                 const gchar            *id,
                                                                 const gchar            *type);
static gchar           *exo_toolbars_model_real_get_item_type   (ExoToolbarsModel       *model,
                                                                 GdkAtom                 dnd_type);
static gchar           *exo_toolbars_model_real_get_item_id     (ExoToolbarsModel       *model,
                                                                 const gchar            *type,
                                                                 const gchar            *data);
static gchar           *exo_toolbars_model_real_get_item_data   (ExoToolbarsModel       *model,
                                                                 const gchar            *type,
                                                                 const gchar            *id);
static gboolean         exo_toolbars_model_has_action           (ExoToolbarsModel       *model,
                                                                 const gchar            *action);
static ExoToolbarsItem *exo_toolbars_item_new                   (const gchar            *id,
                                                                 const gchar            *type,
                                                                 gboolean                is_separator);
static void             exo_toolbars_toolbar_free               (ExoToolbarsToolbar     *toolbar);



struct _ExoToolbarsModelPrivate
{
  gchar **actions;
  GList  *toolbars;
};

struct _ExoToolbarsToolbar
{
  ExoToolbarsModelFlags flags;
  GtkToolbarStyle       style;
  GList                *items;
  gchar                *name;
};

struct _ExoToolbarsItem
{
  gchar    *id;
  gchar    *type;
  gboolean  is_separator;
};



static GObjectClass *parent_class;
static guint         toolbars_model_signals[LAST_SIGNAL];



G_DEFINE_TYPE (ExoToolbarsModel, exo_toolbars_model, G_TYPE_OBJECT);



static gboolean
_exo_accumulator_STRING (GSignalInvocationHint *hint,
                         GValue                *return_accu,
                         const GValue          *handler_return,
                         gpointer               dummy)
{
  const gchar *retval;
  retval = g_value_get_string (handler_return);
  g_value_set_string (return_accu, retval);
  return (retval == NULL || *retval == '\0');
}



static void
exo_toolbars_model_class_init (ExoToolbarsModelClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoToolbarsModelPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_toolbars_model_finalize;

  klass->add_item = exo_toolbars_model_real_add_item;
  klass->get_item_id = exo_toolbars_model_real_get_item_id;
  klass->get_item_data = exo_toolbars_model_real_get_item_data;
  klass->get_item_type = exo_toolbars_model_real_get_item_type;

  /**
   * ExoToolbarsModel::item-added:
   **/
  toolbars_model_signals[ITEM_ADDED] =
    g_signal_new ("item-added",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, item_added),
                  NULL, NULL,
                  _exo_marshal_VOID__INT_INT,
                  G_TYPE_NONE, 2,
                  G_TYPE_INT,
                  G_TYPE_INT);

  /**
   * ExoToolbarsModel::item-removed:
   **/
  toolbars_model_signals[ITEM_REMOVED] =
    g_signal_new ("item-removed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, item_removed),
                  NULL, NULL,
                  _exo_marshal_VOID__INT_INT,
                  G_TYPE_NONE, 2,
                  G_TYPE_INT,
                  G_TYPE_INT);

  /**
   * ExoToolbarsModel::toolbar-added:
   **/
  toolbars_model_signals[TOOLBAR_ADDED] =
    g_signal_new ("toolbar-added",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, toolbar_added),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  /**
   * ExoToolbarsModel::toolbar-changed:
   **/
  toolbars_model_signals[TOOLBAR_CHANGED] =
    g_signal_new ("toolbar-changed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, toolbar_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  /**
   * ExoToolbarsModel::toolbar-removed:
   **/
  toolbars_model_signals[TOOLBAR_REMOVED] =
    g_signal_new ("toolbar-removed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, toolbar_removed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  /**
   * ExoToolbarsModel::get-item-type:
   **/
  toolbars_model_signals[GET_ITEM_TYPE] =
    g_signal_new ("get-item-type",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, get_item_type),
                  _exo_accumulator_STRING, NULL,
                  _exo_marshal_STRING__POINTER,
                  G_TYPE_STRING, 1,
                  G_TYPE_POINTER);

  /**
   * ExoToolbarsModel::get-item-id:
   **/
  toolbars_model_signals[GET_ITEM_ID] =
    g_signal_new ("get-item-id",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, get_item_id),
                  _exo_accumulator_STRING, NULL,
                  _exo_marshal_STRING__STRING_STRING,
                  G_TYPE_STRING, 2,
                  G_TYPE_STRING,
                  G_TYPE_STRING);

  /**
   * ExoToolbarsModel::get-item-data:
   **/
  toolbars_model_signals[GET_ITEM_DATA] =
    g_signal_new ("get-item-data",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, get_item_data),
                  _exo_accumulator_STRING, NULL,
                  _exo_marshal_STRING__STRING_STRING,
                  G_TYPE_STRING, 2,
                  G_TYPE_STRING,
                  G_TYPE_STRING);
}



static void
exo_toolbars_model_init (ExoToolbarsModel *model)
{
  model->priv = EXO_TOOLBARS_MODEL_GET_PRIVATE (model);
}



static void
exo_toolbars_model_finalize (GObject *object)
{
  ExoToolbarsModel *model = EXO_TOOLBARS_MODEL (object);
  GList            *lp;

  if (G_LIKELY (model->priv->actions != NULL))
    g_strfreev (model->priv->actions);

  for (lp = model->priv->toolbars; lp != NULL; lp = lp->next)
    exo_toolbars_toolbar_free (lp->data);
  g_list_free (model->priv->toolbars);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}



static gboolean
exo_toolbars_model_real_add_item (ExoToolbarsModel *model,
                                  gint              toolbar_position,
                                  gint              item_position,
                                  const gchar      *id,
                                  const gchar      *type)
{
  ExoToolbarsToolbar *toolbar;
  ExoToolbarsItem    *item;
  gint                item_index;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), FALSE);
  g_return_val_if_fail (type != NULL, FALSE);
  g_return_val_if_fail (id != NULL, FALSE);
  g_return_val_if_fail (exo_toolbars_model_has_action (model, id), FALSE);

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_val_if_fail (toolbar != NULL, FALSE);

  item = exo_toolbars_item_new (id, type, FALSE);
  toolbar->items = g_list_insert (toolbar->items, item, item_position);

  item_index = g_list_index (toolbar->items, item);
  g_signal_emit (G_OBJECT (model), toolbars_model_signals[ITEM_ADDED], 0,
                 toolbar_position, item_index);

  return TRUE;
}



static gchar*
exo_toolbars_model_real_get_item_type (ExoToolbarsModel *model,
                                       GdkAtom           dnd_type)
{
  if (gdk_atom_intern (EXO_TOOLBARS_ITEM_TYPE, FALSE) == dnd_type)
    return g_strdup (EXO_TOOLBARS_ITEM_TYPE);
  return NULL;
}



static gchar*
exo_toolbars_model_real_get_item_id (ExoToolbarsModel *model,
                                     const gchar      *type,
                                     const gchar      *data)
{
  if (exo_str_is_equal (type, EXO_TOOLBARS_ITEM_TYPE))
    return g_strdup (data);
  return NULL;
}



static gchar*
exo_toolbars_model_real_get_item_data (ExoToolbarsModel *model,
                                       const gchar      *type,
                                       const gchar      *id)
{
  if (exo_str_is_equal (type, EXO_TOOLBARS_ITEM_TYPE))
    return g_strdup (id);
  return NULL;
}



static gboolean
exo_toolbars_model_has_action (ExoToolbarsModel *model,
                               const gchar      *action)
{
  guint n;

  if (G_LIKELY (model->priv->actions != NULL))
    {
      for (n = 0; model->priv->actions[n] != NULL; ++n)
        if (exo_str_is_equal (action, model->priv->actions[n]))
          return TRUE;
    }

  return FALSE;
}



static ExoToolbarsItem*
exo_toolbars_item_new (const gchar *id,
                       const gchar *type,
                       gboolean     is_separator)
{
  ExoToolbarsItem *item;

  item = g_new (ExoToolbarsItem, 1);
  item->id = g_strdup (id);
  item->type = g_strdup (type);
  item->is_separator = is_separator;

  return item;
}



static void
exo_toolbars_toolbar_free (ExoToolbarsToolbar *toolbar)
{
  ExoToolbarsItem *item;
  GList           *lp;

  for (lp = toolbar->items; lp != NULL; lp = lp->next)
    {
      item = lp->data;
      g_free (item->type);
      g_free (item->id);
      g_free (item);
    }
  
  g_list_free (toolbar->items);
  g_free (toolbar->name);
  g_free (toolbar);
}



/**
 * exo_toolbars_model_new:
 *
 * Creates a new #ExoToolbarsModel.
 *
 * Return value : A newly created #ExoToolbarsModel.
 **/
ExoToolbarsModel*
exo_toolbars_model_new (void)
{
  return g_object_new (EXO_TYPE_TOOLBARS_MODEL, NULL);
}



/**
 * exo_toolbars_model_set_actions:
 **/
void
exo_toolbars_model_set_actions (ExoToolbarsModel      *model,
                                gchar                **actions,
                                guint                  n_actions)
{
  guint n;
  
  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));
  g_return_if_fail (actions != NULL);

  if (model->priv->toolbars != NULL)
    {
      g_warning ("exo_toolbars_model_set_actions must be called before "
                 "you add toolbars to the model.");
      return;
    }

  if (model->priv->actions != NULL)
    {
      g_warning ("exo_toolbars_model_set_actions can only be called once");
      return;
    }

  model->priv->actions = g_new (gchar*, n_actions + 1);
  for (n = 0; n < n_actions; ++n)
    model->priv->actions[n] = g_strdup (actions[n]);
  model->priv->actions[n] = NULL;
}



/**
 * exo_toolbars_model_get_actions:
 **/
gchar**
exo_toolbars_model_get_actions (ExoToolbarsModel *model)
{
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), NULL);
  return (model->priv->actions != NULL) ? g_strdupv (model->priv->actions) : NULL;
}



/**
 * exo_toolbars_model_load_from_file:
 * @model       : An #ExoToolbarsModel.
 * @filename    :
 * @error       :
 *
 * Return value :
 **/
gboolean
exo_toolbars_model_load_from_file (ExoToolbarsModel *model,
                                   const gchar      *filename,
                                   GError          **error)
{
  xmlNodePtr node;
  xmlNodePtr child;
  xmlDocPtr  doc;
  xmlChar   *style;
  xmlChar   *name;
  xmlChar   *type;
  xmlChar   *id;
  gint       index;
  
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  doc = xmlParseFile (filename);
  if (G_UNLIKELY (doc == NULL))
    {
      g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_PARSE,
                   "Unable to parse file %s", filename);
      return FALSE;
    }

  for (node = xmlDocGetRootElement (doc)->children; node != NULL; node = node->next)
    if (xmlStrEqual (node->name, (const xmlChar *) "toolbar"))
      {
        name = xmlGetProp (node, (const xmlChar *) "name");
        if (G_UNLIKELY (name == NULL))
          continue;

        index = exo_toolbars_model_add_toolbar (model, -1, (const gchar *) name);
        xmlFree (name);

        style = xmlGetProp (node, (const xmlChar *) "style");
        if (style != NULL)
          {
            if (exo_str_is_equal ((const gchar *) style, "icons"))
              exo_toolbars_model_set_style (model, GTK_TOOLBAR_ICONS, index);
            else if (exo_str_is_equal ((const gchar *) style, "text"))
              exo_toolbars_model_set_style (model, GTK_TOOLBAR_TEXT, index);
            else if (exo_str_is_equal ((const gchar *) style, "both"))
              exo_toolbars_model_set_style (model, GTK_TOOLBAR_BOTH, index);
            else if (exo_str_is_equal ((const gchar *) style, "both-horiz"))
              exo_toolbars_model_set_style (model, GTK_TOOLBAR_BOTH_HORIZ, index);

            xmlFree (style);
          }

        for (child = node->children; child != NULL; child = child->next)
          {
            if (xmlStrEqual (child->name, (const xmlChar *) "toolitem"))
              {
                id = xmlGetProp (child, (const xmlChar *) "id");
                if (G_LIKELY (id != NULL))
                  {
                    type = xmlGetProp (child, (const xmlChar *) "type");
                    if (G_UNLIKELY (type == NULL))
                      type = xmlStrdup (EXO_TOOLBARS_ITEM_TYPE);
                    exo_toolbars_model_add_item (model, index, -1, id, type);
                    xmlFree (type);
                    xmlFree (id);
                  }
              }
            else if (xmlStrEqual (child->name, (const xmlChar *) "separator"))
              {
                exo_toolbars_model_add_separator (model, index, -1);
              }
          }
      }

  xmlFreeDoc (doc);

  return TRUE;
}



/**
 * exo_toolbars_model_save_to_file:
 * @model       : An #ExoToolbarsModel.
 * @filename    :
 * @error       :
 *
 * Return value :
 **/
gboolean
exo_toolbars_model_save_to_file (ExoToolbarsModel *model,
                                 const gchar      *filename,
                                 GError          **error)
{
  ExoToolbarsToolbar   *toolbar;
  ExoToolbarsItem      *item;
  GList                *tp;
  GList                *ip;
  FILE                 *fp;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  fp = fopen (filename, "w");
  if (G_UNLIKELY (fp == NULL))
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   "Unable to open file %s for writing: %s", filename,
                   g_strerror (errno));
      return FALSE;
    }

  fprintf (fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf (fp, "<!DOCTYPE toolbars SYSTEM \"toolbars.dtd\">\n\n");
  fprintf (fp, "<toolbars>\n");

  for (tp = model->priv->toolbars; tp != NULL; tp = tp->next)
    {
      toolbar = tp->data;
      fprintf (fp, "  <toolbar name=\"%s\"", toolbar->name);
      if ((toolbar->flags & EXO_TOOLBARS_MODEL_OVERRIDE_STYLE) != 0)
        {
          switch (toolbar->style)
            {
            case GTK_TOOLBAR_ICONS:
              fprintf (fp, " style=\"icons\"");
              break;

            case GTK_TOOLBAR_TEXT:
              fprintf (fp, " style=\"text\"");
              break;

            case GTK_TOOLBAR_BOTH:
              fprintf (fp, " style=\"both\"");
              break;

            case GTK_TOOLBAR_BOTH_HORIZ:
              fprintf (fp, " style=\"both-horiz\"");
              break;
            }
        }
      fprintf (fp, ">\n");

      for (ip = toolbar->items; ip != NULL; ip = ip->next)
        {
          item = ip->data;
          if (item->is_separator)
            {
              fprintf (fp, "    <separator />\n");
            }
          else
            {
              fprintf (fp, "    <toolitem id=\"%s\" type=\"%s\" />\n",
                       item->id, item->type);
            }
        }

      fprintf (fp, "  </toolbar>\n");
    }

  fprintf (fp, "</toolbars>\n");
  fclose (fp);

  return TRUE;
}



/**
 * exo_toolbars_model_get_style:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  : The index of a toolbar in @model.
 *
 * Returns the overridden #GtkToolbarStyle for the toolbar
 * at @toolbar_position. Should only be used if
 * %EXO_TOOLBARS_MODEL_OVERRIDE_STYLE is set for the
 * toolbar.
 *
 * Return value       : The #GtkToolbarStyle associated with
 *                      @toolbar_position.
 **/
GtkToolbarStyle
exo_toolbars_model_get_style (ExoToolbarsModel *model,
                              gint              toolbar_position)
{
  ExoToolbarsToolbar *toolbar;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), GTK_TOOLBAR_BOTH);

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_val_if_fail (toolbar != NULL, GTK_TOOLBAR_BOTH);
  g_return_val_if_fail (toolbar->flags & EXO_TOOLBARS_MODEL_OVERRIDE_STYLE, GTK_TOOLBAR_BOTH);

  return toolbar->style;
}



/**
 * exo_toolbars_model_set_style:
 * @model             : An #ExoToolbarsModel.
 * @style             : A #GtkToolbarStyle.
 * @toolbar_position  : The index of a toolbar in @model.
 **/
void
exo_toolbars_model_set_style (ExoToolbarsModel *model,
                              GtkToolbarStyle   style,
                              gint              toolbar_position)
{
  ExoToolbarsToolbar *toolbar;

  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_if_fail (toolbar != NULL);

  if ((toolbar->flags & EXO_TOOLBARS_MODEL_OVERRIDE_STYLE) == 0
      || toolbar->style != style)
    {
      toolbar->flags = toolbar->flags | EXO_TOOLBARS_MODEL_OVERRIDE_STYLE;
      toolbar->style = style;

      g_signal_emit (G_OBJECT (model), toolbars_model_signals[TOOLBAR_CHANGED],
                     0, toolbar_position);
    }
}



/**
 * exo_toolbars_model_unset_style:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  : The index of a toolbar in @model.
 **/
void
exo_toolbars_model_unset_style (ExoToolbarsModel *model,
                                gint              toolbar_position)
{
  ExoToolbarsToolbar *toolbar;
  
  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_if_fail (toolbar != NULL);

  if ((toolbar->flags & EXO_TOOLBARS_MODEL_OVERRIDE_STYLE) != 0)
    {
      toolbar->flags &= ~EXO_TOOLBARS_MODEL_OVERRIDE_STYLE;
      g_signal_emit (G_OBJECT (model), toolbars_model_signals[TOOLBAR_CHANGED],
                     0, toolbar_position);
    }
}



/**
 * exo_toolbars_model_get_flags:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  : The index of a toolbar in @model.
 *
 * Returns the #ExoToolbarsModelFlags associated with the 
 * toolbar at @toolbar_position.
 *
 * Return value       : The #ExoToolbarsModelFlags associated
 *                      with @toolbar_position.
 **/
ExoToolbarsModelFlags
exo_toolbars_model_get_flags (ExoToolbarsModel *model,
                              gint              toolbar_position)
{
  ExoToolbarsToolbar *toolbar;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), 0);

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_val_if_fail (toolbar != NULL, 0);

  return toolbar->flags;
}



/**
 * exo_toolbars_model_set_flags:
 * @model             : An #ExoToolbarsModel.
 * @flags             : The new flags for @toolbar_position.
 * @toolbar_position  : The index of a toolbar in @model.
 *
 * Changes the #ExoToolbarsModelFlags associated with the
 * toolbar at @toolbar_position to the value of @flags.
 **/
void
exo_toolbars_model_set_flags (ExoToolbarsModel      *model,
                              ExoToolbarsModelFlags  flags,
                              gint                   toolbar_position)
{
  ExoToolbarsToolbar *toolbar;

  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_if_fail (toolbar != NULL);

  toolbar->flags = flags;

  g_signal_emit (G_OBJECT (model), toolbars_model_signals[TOOLBAR_CHANGED],
                 0, toolbar_position);
}



/**
 * exo_toolbars_model_get_item_type:
 * @model       : An #ExoToolbarsModel.
 * @dnd_type    :
 *
 * Return value :
 **/
gchar*
exo_toolbars_model_get_item_type (ExoToolbarsModel *model,
                                  GdkAtom           dnd_type)
{
  gchar *result;
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), NULL);
  g_signal_emit (G_OBJECT (model), toolbars_model_signals[GET_ITEM_TYPE], 0, dnd_type, &result);
  return result;
}



/**
 * exo_toolbars_model_get_item_id:
 * @model       : An #ExoToolbarsModel.
 * @type        :
 * @name        :
 *
 * Return value :
 **/
gchar*
exo_toolbars_model_get_item_id (ExoToolbarsModel *model,
                                const gchar      *type,
                                const gchar      *name)
{
  gchar *result;
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), NULL);
  g_signal_emit (G_OBJECT (model), toolbars_model_signals[GET_ITEM_ID], 0, type, name, &result);
  return result;
}



/**
 * exo_toolbars_model_get_item_data:
 * @model       : An #ExoToolbarsModel.
 * @type        :
 * @id          :
 *
 * Return value :
 **/
gchar*
exo_toolbars_model_get_item_data (ExoToolbarsModel *model,
                                  const gchar      *type,
                                  const gchar      *id)
{
  gchar *result;
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), NULL);
  g_signal_emit (G_OBJECT (model), toolbars_model_signals[GET_ITEM_DATA], 0, type, id, &result);
  return result;
}



/**
 * exo_toolbars_model_add_item:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  :
 * @item_position     :
 * @id                :
 * @type              :
 *
 * Return value       :
 **/
gboolean
exo_toolbars_model_add_item (ExoToolbarsModel      *model,
                             gint                   toolbar_position,
                             gint                   item_position,
                             const gchar           *id,
                             const gchar           *type)
{
  ExoToolbarsModelClass *klass = EXO_TOOLBARS_MODEL_GET_CLASS (model);
  return klass->add_item (model, toolbar_position, item_position, id, type);
}



/**
 * exo_toolbars_model_add_separator:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  :
 * @item_position     :
 **/
void
exo_toolbars_model_add_separator (ExoToolbarsModel *model,
                                  gint              toolbar_position,
                                  gint              item_position)
{
  ExoToolbarsToolbar *toolbar;
  ExoToolbarsItem    *item;
  gint                item_index;

  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_assert (toolbar != NULL);

  item = exo_toolbars_item_new ("separator", EXO_TOOLBARS_ITEM_TYPE, TRUE);
  toolbar->items = g_list_insert (toolbar->items, item, item_position);

  item_index = g_list_index (toolbar->items, item);
  g_signal_emit (G_OBJECT (model), toolbars_model_signals[ITEM_ADDED], 0,
                 toolbar_position, item_index);
}



/**
 * exo_toolbars_model_add_toolbar:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  :
 * @name              :
 *
 * Return value       : The real position of the new toolbar in @model.
 **/
gint
exo_toolbars_model_add_toolbar (ExoToolbarsModel *model,
                                gint              toolbar_position,
                                const gchar      *name)
{
  ExoToolbarsToolbar *toolbar;
  gint                toolbar_index;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), -1);
  g_return_val_if_fail (name != NULL, -1);

  toolbar = g_new (ExoToolbarsToolbar, 1);
  toolbar->name = g_strdup (name);
  toolbar->items = NULL;
  toolbar->flags = 0;
  toolbar->style = GTK_TOOLBAR_BOTH;

  model->priv->toolbars = g_list_insert (model->priv->toolbars,
                                         toolbar,
                                         toolbar_position);

  toolbar_index = g_list_index (model->priv->toolbars, toolbar);
  g_signal_emit (G_OBJECT (model), toolbars_model_signals[TOOLBAR_ADDED],
                 0, toolbar_index); 

  return toolbar_index;
}



/**
 * exo_toolbars_model_move_item:
 * @model                 : An #ExoToolbarsModel.
 * @toolbar_position      : Old toolbar index.
 * @item_position         : Old item index.
 * @new_toolbar_position  : New toolbar index.
 * @new_item_position     : New item index.
 *
 * Moves an item to another position.
 **/
void
exo_toolbars_model_move_item (ExoToolbarsModel *model,
                              gint              toolbar_position,
                              gint              item_position,
                              gint              new_toolbar_position,
                              gint              new_item_position)
{
  ExoToolbarsToolbar *new_toolbar;
  ExoToolbarsToolbar *toolbar;
  ExoToolbarsItem    *item;

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_if_fail (toolbar != NULL);

  new_toolbar = g_list_nth_data (model->priv->toolbars, new_toolbar_position);
  g_return_if_fail (new_toolbar != NULL);

  item = g_list_nth_data (toolbar->items, item_position);
  g_return_if_fail (item != NULL);

  toolbar->items = g_list_remove (toolbar->items, item);

  g_signal_emit (G_OBJECT (model), toolbars_model_signals[ITEM_REMOVED],
                 0, toolbar_position, item_position);

  new_toolbar->items = g_list_insert (new_toolbar->items, item, new_item_position);
  new_item_position = g_list_index (new_toolbar->items, item);

  g_signal_emit (G_OBJECT (model), toolbars_model_signals[ITEM_ADDED],
                 0, new_toolbar_position, new_item_position);
}



/**
 * exo_toolbars_model_remove_item:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  : A toolbar index.
 * @item_position     : The index of the item to remove.
 *
 * Removes the toolbar item at @item_position from the toolbar
 * @toolbar_position in @model.
 **/
void
exo_toolbars_model_remove_item (ExoToolbarsModel *model,
                                gint              toolbar_position,
                                gint              item_position)
{
  ExoToolbarsToolbar *toolbar;
  ExoToolbarsItem    *item;

  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_if_fail (toolbar != NULL);

  item = g_list_nth_data (toolbar->items, item_position);
  g_return_if_fail (item != NULL);

  toolbar->items = g_list_remove (toolbar->items, item);
  g_free (item->type);
  g_free (item->id);
  g_free (item);

  g_signal_emit (G_OBJECT (model), toolbars_model_signals[ITEM_REMOVED],
                 0, toolbar_position, item_position);
}



/**
 * exo_toolbars_model_remove_toolbar:
 * @model             : A #ExoToolbarsModel.
 * @toolbar_position  :
 **/
void
exo_toolbars_model_remove_toolbar (ExoToolbarsModel *model,
                                   gint              toolbar_position)
{
  ExoToolbarsToolbar *toolbar;

  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));
  
  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_if_fail (toolbar != NULL);

  if ((toolbar->flags & EXO_TOOLBARS_MODEL_NOT_REMOVABLE) == 0)
    {
      model->priv->toolbars = g_list_remove (model->priv->toolbars, toolbar);
      exo_toolbars_toolbar_free (toolbar);

      g_signal_emit (G_OBJECT (model), toolbars_model_signals[TOOLBAR_REMOVED],
                     0, toolbar_position);
    }
}



/**
 * exo_toolbars_model_n_items:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  :
 *
 * Return value       :
 **/
gint
exo_toolbars_model_n_items (ExoToolbarsModel *model,
                            gint              toolbar_position)
{
  ExoToolbarsToolbar *toolbar;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), -1);

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_val_if_fail (toolbar != NULL, -1);

  return g_list_length (toolbar->items);
}



/**
 * exo_toolbars_model_item_nth:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  :
 * @item_position     :
 * @is_separator      :
 * @id                :
 * @type              :
 **/
void
exo_toolbars_model_item_nth (ExoToolbarsModel *model,
                             gint              toolbar_position,
                             gint              item_position,
                             gboolean         *is_separator,
                             const gchar     **id,
                             const gchar     **type)
{
  ExoToolbarsToolbar *toolbar;
  ExoToolbarsItem    *item;
  
  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model));

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_if_fail (toolbar != NULL);

  item = g_list_nth_data (toolbar->items, item_position);
  g_return_if_fail (item != NULL);

  if (G_LIKELY (is_separator != NULL))
    *is_separator = item->is_separator;

  if (G_LIKELY (type != NULL))
    *type = item->type;

  if (G_LIKELY (id != NULL))
    *id = item->id;
}



/**
 * exo_toolbars_model_n_toolbars:
 * @model       : An #ExoToolbarsModel.
 *
 * Return value : The number of toolbars in @model.
 **/
gint
exo_toolbars_model_n_toolbars (ExoToolbarsModel *model)
{
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), -1);
  return g_list_length (model->priv->toolbars);
}



/**
 * exo_toolbars_model_toolbar_nth:
 * @model             : An #ExoToolbarsModel.
 * @toolbar_position  :
 *
 * Return value       :
 **/
const gchar*
exo_toolbars_model_toolbar_nth (ExoToolbarsModel *model,
                                gint              toolbar_position)
{
  ExoToolbarsToolbar *toolbar;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), NULL);

  toolbar = g_list_nth_data (model->priv->toolbars, toolbar_position);
  g_return_val_if_fail (toolbar != NULL, NULL);

  return toolbar->name;
}


