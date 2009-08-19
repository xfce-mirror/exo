/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
 * Copyright (c) 2003      Marco Pesenti Gritti
 *
 * Written by Benedikt Meurer <benny@xfce.org>.
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

#include <libxfce4util/libxfce4util.h>

#include <exo/exo-marshal.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-toolbars-model.h>
#include <exo/exo-toolbars-private.h>
#include <exo/exo-alias.h>



#define EXO_TOOLBARS_MODEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_TOOLBARS_MODEL, ExoToolbarsModelPrivate))



typedef struct _ExoToolbarsToolbar ExoToolbarsToolbar;
typedef struct _ExoToolbarsItem    ExoToolbarsItem;
typedef struct _UiParser           UiParser;



typedef enum
{
  UI_PARSER_START,
  UI_PARSER_TOOLBARS,
  UI_PARSER_TOOLBAR,
  UI_PARSER_TOOLITEM,
  UI_PARSER_SEPARATOR,
} UiParserState;

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
static void             start_element_handler                   (GMarkupParseContext    *context,
                                                                 const gchar            *element_name,
                                                                 const gchar           **attribute_names,
                                                                 const gchar           **attribute_values,
                                                                 gpointer                user_data,
                                                                 GError                **error);
static void             end_element_handler                     (GMarkupParseContext    *context,
                                                                 const gchar            *element_name,
                                                                 gpointer                user_data,
                                                                 GError                **error);



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

typedef XFCE_GENERIC_STACK(UiParserState) UiParserStack;

struct _UiParser
{
  UiParserStack     *stack;

  ExoToolbarsModel  *model;
  gint               toolbar_position;
};



static guint toolbars_model_signals[LAST_SIGNAL];



G_DEFINE_TYPE (ExoToolbarsModel, exo_toolbars_model, G_TYPE_OBJECT)



static gboolean
_exo_accumulator_STRING (GSignalInvocationHint *hint,
                         GValue                *return_accu,
                         const GValue          *handler_return,
                         gpointer               dummy)
{
  const gchar *retval;
  retval = g_value_get_string (handler_return);
  g_value_set_string (return_accu, retval);
  return exo_str_is_empty (retval);
}



static void
exo_toolbars_model_class_init (ExoToolbarsModelClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoToolbarsModelPrivate));

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_toolbars_model_finalize;

  klass->add_item = exo_toolbars_model_real_add_item;
  klass->get_item_id = exo_toolbars_model_real_get_item_id;
  klass->get_item_data = exo_toolbars_model_real_get_item_data;
  klass->get_item_type = exo_toolbars_model_real_get_item_type;

  /**
   * ExoToolbarsModel::item-added:
   * @model             : The #ExoToolbarsModel to which an item was added.
   * @toolbar_position  : The index of the toolbar in @model to which the item
   *                      was added.
   * @item_position     : The index of the new item in the specified toolbar.
   *
   * This signal is emitted whenever a new item is added to a toolbar
   * managed by @model.
   **/
  toolbars_model_signals[ITEM_ADDED] =
    g_signal_new (I_("item-added"),
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
   * @model             : The #ExoToolbarsModel from which an item was removed.
   * @toolbar_position  : The index of the toolbar in @model from which
   *                      the item was removed.
   * @item_position     : The index of the item in the specified toolbar.
   *
   * This signal is emitted whenever an item is removed from a toolbar
   * managed by @model.
   **/
  toolbars_model_signals[ITEM_REMOVED] =
    g_signal_new (I_("item-removed"),
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
   * @model             : The #ExoToolbarsModel to which a new toolbar was
   *                      added.
   * @toolbar_position  : The index of the new toolbar in @model.
   *
   * This signal is emitted whenever a new toolbar is added to @model.
   **/
  toolbars_model_signals[TOOLBAR_ADDED] =
    g_signal_new (I_("toolbar-added"),
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, toolbar_added),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  /**
   * ExoToolbarsModel::toolbar-changed:
   * @model             : The #ExoToolbarsModel that manages the changed
   *                      toolbar.
   * @toolbar_position  : The index of the changed toolbar in @model.
   *
   * This signal is emitted whenever the flags or the style of a toolbar
   * change, which is managed by @model. All views connected to @model
   * should then update their internal state of the specified toolbar.
   **/
  toolbars_model_signals[TOOLBAR_CHANGED] =
    g_signal_new (I_("toolbar-changed"),
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoToolbarsModelClass, toolbar_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  /**
   * ExoToolbarsModel::toolbar-removed:
   * @model             : The #ExoToolbarsModel
   * @toolbar_position  : The index of the toolbar in @model that was
   *                      removed.
   *
   * This signal is emitted whenever a toolbar is removed from @model.
   **/
  toolbars_model_signals[TOOLBAR_REMOVED] =
    g_signal_new (I_("toolbar-removed"),
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
    g_signal_new (I_("get-item-type"),
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
    g_signal_new (I_("get-item-id"),
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
    g_signal_new (I_("get-item-data"),
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

  (*G_OBJECT_CLASS (exo_toolbars_model_parent_class)->finalize) (object);
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

  if (!exo_toolbars_model_has_action (model, id))
    {
      g_warning ("Tried to add action \"%s\" to an ExoToolbarsModel, "
                 "which does not include \"%s\".", id, id);
      return FALSE;
    }

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

  item = g_slice_new (ExoToolbarsItem);
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
      g_slice_free (ExoToolbarsItem, item);
    }

  g_list_free (toolbar->items);
  g_free (toolbar->name);
  g_slice_free (ExoToolbarsToolbar, toolbar);
}



static void
start_element_handler (GMarkupParseContext  *context,
                       const gchar          *element_name,
                       const gchar         **attribute_names,
                       const gchar         **attribute_values,
                       gpointer              user_data,
                       GError              **error)
{
  const gchar *style_prop = NULL;
  const gchar *name_prop = NULL;
  const gchar *type_prop = EXO_TOOLBARS_ITEM_TYPE;
  const gchar *id_prop = NULL;
  UiParser    *parser = user_data;
  guint        n;

  switch (xfce_stack_top (parser->stack))
    {
    case UI_PARSER_START:
      if (exo_str_is_equal (element_name, "toolbars"))
        xfce_stack_push (parser->stack, UI_PARSER_TOOLBARS);
      else
        goto unknown_element;
      break;

    case UI_PARSER_TOOLBARS:
      if (exo_str_is_equal (element_name, "toolbar"))
        {
          /* find name/style attributes */
          for (n = 0; attribute_names[n] != NULL; ++n)
            {
              if (exo_str_is_equal (attribute_names[n], "name"))
                name_prop = attribute_values[n];
              else if (exo_str_is_equal (attribute_names[n], "style"))
                style_prop = attribute_values[n];
            }

          /* name is required */
          if (G_UNLIKELY (name_prop == NULL))
            {
              g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT,
                           "Element <toolbar> requires an attribute name");
              return;
            }

          /* create the toolbar */
          parser->toolbar_position = exo_toolbars_model_add_toolbar (parser->model, -1, name_prop);

          /* set style if given */
          if (exo_str_is_equal (style_prop, "icons"))
            exo_toolbars_model_set_style (parser->model, GTK_TOOLBAR_ICONS, parser->toolbar_position);
          else if (exo_str_is_equal (style_prop, "text"))
            exo_toolbars_model_set_style (parser->model, GTK_TOOLBAR_TEXT, parser->toolbar_position);
          else if (exo_str_is_equal (style_prop, "both"))
            exo_toolbars_model_set_style (parser->model, GTK_TOOLBAR_BOTH, parser->toolbar_position);
          else if (exo_str_is_equal (style_prop, "both-horiz"))
            exo_toolbars_model_set_style (parser->model, GTK_TOOLBAR_BOTH_HORIZ, parser->toolbar_position);

          xfce_stack_push (parser->stack, UI_PARSER_TOOLBAR);
        }
      else
        goto unknown_element;
      break;

    case UI_PARSER_TOOLBAR:
      if (exo_str_is_equal (element_name, "toolitem"))
        {
          /* find id/type attributes */
          for (n = 0; attribute_names[n] != NULL; ++n)
            {
              if (exo_str_is_equal (attribute_names[n], "id"))
                id_prop = attribute_values[n];
              else if (exo_str_is_equal (attribute_names[n], "type"))
                type_prop = attribute_values[n];
            }

          /* id is required */
          if (G_UNLIKELY (id_prop == NULL))
            {
              g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT,
                           "Element <toolitem> requires an attribute id");
              return;
            }

          /* add the new toolitem */
          exo_toolbars_model_add_item (parser->model, parser->toolbar_position,
                                       -1, id_prop, type_prop);

          xfce_stack_push (parser->stack, UI_PARSER_TOOLITEM);
        }
      else if (exo_str_is_equal (element_name, "separator"))
        {
          /* add the new separator */
          exo_toolbars_model_add_separator (parser->model, parser->toolbar_position, -1);

          xfce_stack_push (parser->stack, UI_PARSER_SEPARATOR);
        }
      else
        goto unknown_element;
      break;

    default:
      goto unknown_element;
    }

  return;

unknown_element:
  g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_UNKNOWN_ELEMENT,
               "Unknown element <%s>", element_name);
}



static void
end_element_handler (GMarkupParseContext  *context,
                     const gchar          *element_name,
                     gpointer              user_data,
                     GError              **error)
{
  UiParser *parser = user_data;

  switch (xfce_stack_top (parser->stack))
    {
    case UI_PARSER_START:
      g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT,
                   "End element handler called while in root context");
      return;

    case UI_PARSER_TOOLBARS:
      if (!exo_str_is_equal (element_name, "toolbars"))
        goto unknown_element;
      break;

    case UI_PARSER_TOOLBAR:
      if (!exo_str_is_equal (element_name, "toolbar"))
        goto unknown_element;
      break;

    case UI_PARSER_TOOLITEM:
      if (!exo_str_is_equal (element_name, "toolitem"))
        goto unknown_element;
      break;

    case UI_PARSER_SEPARATOR:
      if (!exo_str_is_equal (element_name, "separator"))
        goto unknown_element;
      break;

    default:
      goto unknown_element;
    }

  xfce_stack_pop (parser->stack);
  return;

unknown_element:
  g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_UNKNOWN_ELEMENT,
               "Unknown closing element <%s>", element_name);
}



/**
 * exo_toolbars_model_new:
 *
 * Creates a new #ExoToolbarsModel with a reference count
 * of one.
 *
 * You need to call exo_toolbars_model_set_actions() first, after
 * you created an #ExoToolbarsModel to set the list of actions,
 * that should be available from the toolbars.
 *
 * Return value: A newly created #ExoToolbarsModel.
 **/
ExoToolbarsModel*
exo_toolbars_model_new (void)
{
  return g_object_new (EXO_TYPE_TOOLBARS_MODEL, NULL);
}



/**
 * exo_toolbars_model_set_actions:
 * @model     : An #ExoToolbarsModel.
 * @actions   : A string array with action names.
 * @n_actions : The number of strings in @actions.
 *
 * Specifies the list of valid actions for @model. @model will only
 * manage actions that are specified in this list. This function
 * should be called right after you created @model.
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
 * @model : An #ExoToolbarsModel.
 *
 * Returns the list of valid actions for @model.
 *
 * Return value: The list of valid actions for @model.
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
 * @filename    : The name of the file to parse.
 * @error       : Return location for an error or %NULL.
 *
 * Parses a file containing a toolbars UI definition and merges it with
 * the current contents of @model.
 *
 * Return value: %TRUE if the data was successfully loaded from the file
 *               specified by @filename, else %FALSE.
 **/
gboolean
exo_toolbars_model_load_from_file (ExoToolbarsModel *model,
                                   const gchar      *filename,
                                   GError          **error)
{
  const GMarkupParser markup_parser =
  {
    start_element_handler,
    end_element_handler,
    NULL,
    NULL,
    NULL,
  };

  GMarkupParseContext *context;
  UiParser             parser;
  gboolean             succeed;
  gchar               *content;
  gsize                content_len;

  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  /* read the file into memory */
  if (!g_file_get_contents (filename, &content, &content_len, error))
    return FALSE;

  /* initialize the parser */
  parser.stack = xfce_stack_new (UiParserStack);
  parser.model = model;
  xfce_stack_push (parser.stack, UI_PARSER_START);

  /* parse the file */
  context = g_markup_parse_context_new (&markup_parser, 0, &parser, NULL);
  succeed = g_markup_parse_context_parse (context, content, content_len, error)
         && g_markup_parse_context_end_parse (context, error);

  /* cleanup */
  g_markup_parse_context_free (context);
  xfce_stack_free (parser.stack);
  g_free (content);

  return succeed;
}



/**
 * exo_toolbars_model_save_to_file:
 * @model       : An #ExoToolbarsModel.
 * @filename    : The name of the file to save to.
 * @error       : The return location for an error or %NULL.
 *
 * Stores the UI definition of the contents of @model to the file
 * specified by @filename.
 *
 * Return value: %TRUE if saving was successfully, else %FALSE is
 *               returned.
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
  fprintf (fp, "<!-- Autogenerated by %s -->\n\n", PACKAGE_STRING);
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
 * Return value: The #GtkToolbarStyle associated with
 *               @toolbar_position.
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
 *
 * Sets the style to use for a particular toolbar in @model. You can
 * undo the effect of this function by calling
 * exo_toolbars_model_unset_style().
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
 *
 * Undoes the effect of exo_toolbars_model_unset_style() and resets
 * the style of the specified toolbar to the system default.
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
 * Return value: The #ExoToolbarsModelFlags associated
 *               with @toolbar_position.
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
 * @toolbar_position  : The index of toolbar in @model.
 * @item_position     : The position in the specified toolbar or -1.
 * @id                : The identifier of the new item.
 * @type              : The type of the new item.
 *
 * Adds a new toolbar item with the specified @type and @id to @model,
 * where @id has to be a valid action name for @model, that was previously
 * set with exo_toolbars_model_set_actions().
 *
 * Return value: %TRUE if the item was added successfully, else %FALSE.
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
 * @toolbar_position  : The index of a toolbar in @model.
 * @item_position     : The position in the specified toolbar or -1.
 *
 * Adds a new separator item to the specified toolbar in @model. If
 * you specify -1 for @item_position, the separator will be appended
 * to the toolbar, else it will be inserted at the specified @item_position.
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
 * @toolbar_position  : Where to insert the new toolbar in @model
 *                      or -1 to append the toolbar.
 * @name              : The name of the new toolbar.
 *
 * Adds a new toolbar to @model. If you specify -1 for @toolbar_position,
 * the toolbar will be appended to @model; else the toolbar will be
 * inserted at the specified position. Emits the ::toolbar-added
 * signal.
 *
 * Return value: The real position of the new toolbar in @model.
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

  toolbar = g_slice_new (ExoToolbarsToolbar);
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
 * Moves an item to another position. The move operation
 * is done by first removing the specified item and afterwards
 * readding the item at the new position. Therefore, this
 * functions emits the ::item-removed and ::item-added
 * signals.
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
 * @toolbar_position in @model and emits the ::item-removed
 * signal.
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
  g_slice_free (ExoToolbarsItem, item);

  g_signal_emit (G_OBJECT (model), toolbars_model_signals[ITEM_REMOVED],
                 0, toolbar_position, item_position);
}



/**
 * exo_toolbars_model_remove_toolbar:
 * @model             : A #ExoToolbarsModel.
 * @toolbar_position  : The index of a toolbar in @model.
 *
 * Removes the specified toolbar from @model and emits
 * the ::toolbar-removed signal.
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
 * @toolbar_position  : The index of a toolbar in @model.
 *
 * Returns the number of items in the specified toolbar.
 *
 * Return value: The number of items in the specified toolbar.
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
 * @toolbar_position  : The index of a toolbar in @model.
 * @item_position     : The index of an item in the specified toolbar.
 * @is_separator      : Return location for the separator setting or %NULL.
 * @id                : Return location for the item id or %NULL.
 * @type              : Return location for the item type or %NULL.
 *
 * Queries the properites of the toolbar item at @item_position in toolbar
 * @toolbar_position.
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
 * @model : An #ExoToolbarsModel.
 *
 * Returns the number of toolbars currently
 * managed by @model.
 *
 * Return value: The number of toolbars in @model.
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
 * @toolbar_position  : The index of a toolbar in @model.
 *
 * Returns the name of the toolbar at @toolbar_position in
 * @model.
 *
 * Return value: The name of the toolbar at @toolbar_position
 *               in @model.
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



#define __EXO_TOOLBARS_MODEL_C__
#include <exo/exo-aliasdef.c>
