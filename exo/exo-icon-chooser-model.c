/* $Id$ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo-icon-chooser-model.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-alias.h>



typedef struct _ExoIconChooserModelItem ExoIconChooserModelItem;



static void               exo_icon_chooser_model_tree_model_init    (GtkTreeModelIface         *iface);
static void               exo_icon_chooser_model_finalize           (GObject                   *object);
static GtkTreeModelFlags  exo_icon_chooser_model_get_flags          (GtkTreeModel              *tree_model);
static gint               exo_icon_chooser_model_get_n_columns      (GtkTreeModel              *tree_model);
static GType              exo_icon_chooser_model_get_column_type    (GtkTreeModel              *tree_model,
                                                                     gint                       idx);
static gboolean           exo_icon_chooser_model_get_iter           (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreePath               *path);
static GtkTreePath       *exo_icon_chooser_model_get_path           (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static void               exo_icon_chooser_model_get_value          (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     gint                       column,
                                                                     GValue                    *value);
static gboolean           exo_icon_chooser_model_iter_next          (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static gboolean           exo_icon_chooser_model_iter_children      (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreeIter               *parent);
static gboolean           exo_icon_chooser_model_iter_has_child     (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static gint               exo_icon_chooser_model_iter_n_children    (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static gboolean           exo_icon_chooser_model_iter_nth_child     (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreeIter               *parent,
                                                                     gint                       n);
static gboolean           exo_icon_chooser_model_iter_parent        (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreeIter               *child);
static void               exo_icon_chooser_model_icon_theme_changed (GtkIconTheme              *icon_theme,
                                                                     ExoIconChooserModel       *model);
static gint               exo_icon_chooser_model_item_compare       (gconstpointer              item_a,
                                                                     gconstpointer              item_b);
static void               exo_icon_chooser_model_item_free          (ExoIconChooserModelItem   *item);



struct _ExoIconChooserModelClass
{
  GObjectClass __parent__;
};

struct _ExoIconChooserModel
{
  GObject       __parent__;
  GtkIconTheme *icon_theme;
  GList        *items;
  gint          stamp;
};

struct _ExoIconChooserModelItem
{
  ExoIconChooserContext context;
  gchar                *icon_name;
  gchar                *display_name;
};



static const gchar CONTEXT_NAMES[][14] =
{
  "Actions",        /* EXO_ICON_CHOOSER_CONTEXT_ACTIONS */
  "Animations",     /* EXO_ICON_CHOOSER_CONTEXT_ANIMATIONS */
  "Applications",   /* EXO_ICON_CHOOSER_CONTEXT_APPLICATIONS */
  "Categories",     /* EXO_ICON_CHOOSER_CONTEXT_CATEGORIES */
  "Devices",        /* EXO_ICON_CHOOSER_CONTEXT_DEVICES */
  "Emblems",        /* EXO_ICON_CHOOSER_CONTEXT_EMBLEMS */
  "Emotes",         /* EXO_ICON_CHOOSER_CONTEXT_EMOTES */
  "International",  /* EXO_ICON_CHOOSER_CONTEXT_INTERNATIONAL */
  "MimeTypes",      /* EXO_ICON_CHOOSER_CONTEXT_MIME_TYPES */
  "Places",         /* EXO_ICON_CHOOSER_CONTEXT_PLACES */
  "Status",         /* EXO_ICON_CHOOSER_CONTEXT_STATUS */
};



G_DEFINE_TYPE_WITH_CODE (ExoIconChooserModel, exo_icon_chooser_model, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, exo_icon_chooser_model_tree_model_init))



static void
exo_icon_chooser_model_class_init (ExoIconChooserModelClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_icon_chooser_model_finalize;
}



static void
exo_icon_chooser_model_tree_model_init (GtkTreeModelIface *iface)
{
  iface->get_flags = exo_icon_chooser_model_get_flags;
  iface->get_n_columns = exo_icon_chooser_model_get_n_columns;
  iface->get_column_type = exo_icon_chooser_model_get_column_type;
  iface->get_iter = exo_icon_chooser_model_get_iter;
  iface->get_path = exo_icon_chooser_model_get_path;
  iface->get_value = exo_icon_chooser_model_get_value;
  iface->iter_next = exo_icon_chooser_model_iter_next;
  iface->iter_children = exo_icon_chooser_model_iter_children;
  iface->iter_has_child = exo_icon_chooser_model_iter_has_child;
  iface->iter_n_children = exo_icon_chooser_model_iter_n_children;
  iface->iter_nth_child = exo_icon_chooser_model_iter_nth_child;
  iface->iter_parent = exo_icon_chooser_model_iter_parent;
}



static void
exo_icon_chooser_model_init (ExoIconChooserModel *model)
{
  model->stamp = g_random_int ();
}



static void
exo_icon_chooser_model_finalize (GObject *object)
{
  ExoIconChooserModel *model = EXO_ICON_CHOOSER_MODEL (object);

  /* check if we're connected to an icon theme */
  if (G_LIKELY (model->icon_theme != NULL))
    {
      /* disconnect from the icon theme */
      g_signal_handlers_disconnect_by_func (G_OBJECT (model->icon_theme), exo_icon_chooser_model_icon_theme_changed, model);
      g_object_set_data (G_OBJECT (model->icon_theme), "exo-icon-chooser-default-model", NULL);
      g_object_unref (G_OBJECT (model->icon_theme));
    }

  /* release all items */
  g_list_foreach (model->items, (GFunc) exo_icon_chooser_model_item_free, NULL);
  g_list_free (model->items);

  (*G_OBJECT_CLASS (exo_icon_chooser_model_parent_class)->finalize) (object);
}



static GtkTreeModelFlags
exo_icon_chooser_model_get_flags (GtkTreeModel *tree_model)
{
  return GTK_TREE_MODEL_ITERS_PERSIST | GTK_TREE_MODEL_LIST_ONLY;
}



static gint
exo_icon_chooser_model_get_n_columns (GtkTreeModel *tree_model)
{
  return EXO_ICON_CHOOSER_MODEL_N_COLUMNS;
}



static GType
exo_icon_chooser_model_get_column_type (GtkTreeModel *tree_model,
                                        gint          idx)
{
  switch (idx)
    {
    case EXO_ICON_CHOOSER_MODEL_COLUMN_CONTEXT:
      return G_TYPE_UINT;

    case EXO_ICON_CHOOSER_MODEL_COLUMN_ICON_NAME:
    case EXO_ICON_CHOOSER_MODEL_COLUMN_DISPLAY_NAME:
      return G_TYPE_STRING;
    }

  _exo_assert_not_reached ();
  return G_TYPE_INVALID;
}



static gboolean
exo_icon_chooser_model_get_iter (GtkTreeModel *tree_model,
                                 GtkTreeIter  *iter,
                                 GtkTreePath  *path)
{
  ExoIconChooserModel *model = EXO_ICON_CHOOSER_MODEL (tree_model);
  GList               *lp;

  _exo_return_val_if_fail (EXO_IS_ICON_CHOOSER_MODEL (model), FALSE);
  _exo_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

  /* determine the list item for the path */
  lp = g_list_nth (model->items, gtk_tree_path_get_indices (path)[0]);
  if (G_LIKELY (lp != NULL))
    {
      iter->stamp = model->stamp;
      iter->user_data = lp;
      return TRUE;
    }

  return FALSE;
}



static GtkTreePath*
exo_icon_chooser_model_get_path (GtkTreeModel *tree_model,
                                 GtkTreeIter  *iter)
{
  ExoIconChooserModel *model = EXO_ICON_CHOOSER_MODEL (tree_model);
  gint                 idx;

  _exo_return_val_if_fail (EXO_IS_ICON_CHOOSER_MODEL (model), NULL);
  _exo_return_val_if_fail (iter->stamp == model->stamp, NULL);

  /* lookup the list item in the icon list */
  idx = g_list_position (model->items, iter->user_data);
  if (G_LIKELY (idx >= 0))
    return gtk_tree_path_new_from_indices (idx, -1);

  return NULL;
}



static void
exo_icon_chooser_model_get_value (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter,
                                  gint          column,
                                  GValue       *value)
{
  ExoIconChooserModelItem *item;
  ExoIconChooserModel     *model = EXO_ICON_CHOOSER_MODEL (tree_model);
  GtkIconInfo             *info;

  _exo_return_if_fail (EXO_IS_ICON_CHOOSER_MODEL (model));
  _exo_return_if_fail (iter->stamp == model->stamp);

  /* determine the item for the list position */
  item = ((GList *) iter->user_data)->data;

  switch (column)
    {
    case EXO_ICON_CHOOSER_MODEL_COLUMN_CONTEXT:
      g_value_init (value, G_TYPE_UINT);
      g_value_set_uint (value, item->context);
      break;

    case EXO_ICON_CHOOSER_MODEL_COLUMN_ICON_NAME:
      g_value_init (value, G_TYPE_STRING);
      g_value_set_static_string (value, item->icon_name);
      break;

    case EXO_ICON_CHOOSER_MODEL_COLUMN_DISPLAY_NAME:
      /* load the display name on-demand */
      if (G_UNLIKELY (item->display_name == NULL))
        {
          /* determine the icon info */
          info = gtk_icon_theme_lookup_icon (model->icon_theme, item->icon_name, 48, 0);
          if (G_LIKELY (info != NULL))
            {
              /* determine the display name from the icon info */
              item->display_name = g_strdup (gtk_icon_info_get_display_name (info));
              gtk_icon_info_free (info);
            }

          /* fallback to the icon name */
          if (item->display_name == NULL)
            item->display_name = item->icon_name;
        }
      g_value_init (value, G_TYPE_STRING);
      g_value_set_static_string (value, item->display_name);
      break;

    default:
      _exo_assert_not_reached ();
      break;
    }
}



static gboolean
exo_icon_chooser_model_iter_next (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter)
{
  _exo_return_val_if_fail (iter->stamp == EXO_ICON_CHOOSER_MODEL (tree_model)->stamp, FALSE);
  _exo_return_val_if_fail (EXO_IS_ICON_CHOOSER_MODEL (tree_model), FALSE);

  iter->user_data = g_list_next (iter->user_data);
  return (iter->user_data != NULL);
}



static gboolean
exo_icon_chooser_model_iter_children (GtkTreeModel *tree_model,
                                      GtkTreeIter  *iter,
                                      GtkTreeIter  *parent)
{
  ExoIconChooserModel *model = EXO_ICON_CHOOSER_MODEL (tree_model);

  _exo_return_val_if_fail (EXO_IS_ICON_CHOOSER_MODEL (model), FALSE);

  if (G_LIKELY (parent == NULL && model->items != NULL))
    {
      iter->stamp = model->stamp;
      iter->user_data = model->items;
      return TRUE;
    }

  return FALSE;
}



static gboolean
exo_icon_chooser_model_iter_has_child (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter)
{
  return FALSE;
}



static gint
exo_icon_chooser_model_iter_n_children (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter)
{
  ExoIconChooserModel *model = EXO_ICON_CHOOSER_MODEL (tree_model);

  _exo_return_val_if_fail (EXO_IS_ICON_CHOOSER_MODEL (tree_model), 0);

  return (iter == NULL) ? g_list_length (model->items) : 0;
}



static gboolean
exo_icon_chooser_model_iter_nth_child (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter,
                                       GtkTreeIter  *parent,
                                       gint          n)
{
  ExoIconChooserModel *model = EXO_ICON_CHOOSER_MODEL (tree_model);

  _exo_return_val_if_fail (EXO_IS_ICON_CHOOSER_MODEL (tree_model), FALSE);

  if (G_LIKELY (parent == NULL))
    {
      iter->stamp = model->stamp;
      iter->user_data = g_list_nth (model->items, n);
      return (iter->user_data != NULL);
    }

  return FALSE;
}



static gboolean
exo_icon_chooser_model_iter_parent (GtkTreeModel *tree_model,
                                    GtkTreeIter  *iter,
                                    GtkTreeIter  *child)
{
  return FALSE;
}



static void
exo_icon_chooser_model_icon_theme_changed (GtkIconTheme        *icon_theme,
                                           ExoIconChooserModel *model)
{
  ExoIconChooserModelItem *item;
  ExoIconChooserContext    context;
  GtkTreePath             *path;
  GtkTreeIter              iter;
  GList                   *items;
  GList                   *icons;
  GList                   *icp;
  GList                   *itp;

  /* allocate a path to the first model item */
  path = gtk_tree_path_new_from_indices (0, -1);

  /* release all previously loaded icons */
  while (model->items != NULL)
    {
      /* free the first item resources */
      exo_icon_chooser_model_item_free (model->items->data);

      /* remove the item from the list */
      model->items = g_list_delete_link (model->items, model->items);

      /* tell the view that the first item is gone for good */
      gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), path);
    }

  /* determine all icons available for the icon_theme */
  items = gtk_icon_theme_list_icons (icon_theme, NULL);
  for (itp = items; itp != NULL; itp = itp->next)
    {
      /* allocate a model item for the icon... */
      item = g_slice_new (ExoIconChooserModelItem);
      item->context = EXO_ICON_CHOOSER_CONTEXT_OTHER;
      item->icon_name = itp->data;
      item->display_name = NULL;

      /* ...and replace the name with the item */
      itp->data = item;
    }

  /* sort the items by their icon names */
  items = g_list_sort (items, (GCompareFunc) exo_icon_chooser_model_item_compare);

  /* now determine the categories for all items in the model */
  for (context = 0; context < G_N_ELEMENTS (CONTEXT_NAMES); ++context)
    {
      /* determine the icons for the context */
      icons = gtk_icon_theme_list_icons (icon_theme, CONTEXT_NAMES[context]);
      icons = g_list_sort (icons, (GCompareFunc) g_strcasecmp);

      /* update the contexts for the items */
      for (icp = icons, itp = items; icp != NULL && itp != NULL; itp = itp->next)
        {
          /* check if we have a match here */
          item = (ExoIconChooserModelItem *) itp->data;
          if (strcmp (item->icon_name, icp->data) != 0)
            continue;

          /* update the items context */
          item->context = context;
          g_free (icp->data);
          icp = icp->next;
        }

      /* release the icons */
      g_list_foreach (icp, (GFunc) g_free, NULL);
      g_list_free (icons);
    }

  /* insert the items into the model */
  iter.stamp = model->stamp;
  for (itp = g_list_last (items); itp != NULL; itp = itp->prev)
    {
      /* prepend the item to the beginning of our list */
      model->items = g_list_prepend (model->items, itp->data);

      /* setup the iterator for the item */
      iter.user_data = model->items;

      /* tell the view about our new item */
      gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), path, &iter);
    }
  g_list_free (items);

  /* release the path */
  gtk_tree_path_free (path);
}



static gint
exo_icon_chooser_model_item_compare (gconstpointer item_a,
                                     gconstpointer item_b)
{
  return g_strcasecmp (((const ExoIconChooserModelItem *) item_a)->icon_name,
                       ((const ExoIconChooserModelItem *) item_b)->icon_name);
}



static void
exo_icon_chooser_model_item_free (ExoIconChooserModelItem *item)
{
  /* release the display/icon name */
  if (item->display_name != item->icon_name)
    g_free (item->display_name);
  g_free (item->icon_name);

  /* release the item */
  g_slice_free (ExoIconChooserModelItem, item);
}



/**
 * _exo_icon_chooser_model_get_for_widget:
 * @widget : a #GtkWidget.
 *
 * Returns the #ExoIconChooserModel that should be used for the @widget. The
 * caller is responsible to free the returned object using g_object_unref()
 * when no longer needed.
 *
 * Return value: an #ExoIconChooserModel for the @widget.
 *
 * Since: 0.3.1.9
 **/
ExoIconChooserModel*
_exo_icon_chooser_model_get_for_widget (GtkWidget *widget)
{
  GtkIconTheme *icon_theme;

  _exo_return_val_if_fail (GTK_IS_WIDGET (widget), NULL);

  /* determine the icon theme for the widget... */
  icon_theme = gtk_icon_theme_get_for_screen (gtk_widget_get_screen (widget));

  /* ...and return the icon chooser model for the icon theme */
  return _exo_icon_chooser_model_get_for_icon_theme (icon_theme);
}



/**
 * _exo_icon_chooser_model_get_for_icon_theme:
 * @icon_theme : a #GtkIconTheme.
 *
 * Returns an #ExoIconChooserModel for the specified @icon_theme. The
 * caller is responsible to free the returned object using g_object_unref()
 * when no longer needed.
 *
 * Return value: an #ExoIconChooserModel for the @icon_theme.
 *
 * Since: 0.3.1.9
 **/
ExoIconChooserModel*
_exo_icon_chooser_model_get_for_icon_theme (GtkIconTheme *icon_theme)
{
  ExoIconChooserModel *model;

  _exo_return_val_if_fail (GTK_IS_ICON_THEME (icon_theme), NULL);

  /* check if the icon theme is already associated with a model */
  model = g_object_get_data (G_OBJECT (icon_theme), I_("exo-icon-chooser-default-model"));
  if (G_LIKELY (model == NULL))
    {
      /* allocate a new model for the icon theme */
      model = g_object_new (EXO_TYPE_ICON_CHOOSER_MODEL, NULL);
      g_object_set_data (G_OBJECT (icon_theme), "exo-icon-chooser-default-model", model);

      /* associated the model with the icon theme */
      model->icon_theme = g_object_ref (G_OBJECT (icon_theme));
      exo_icon_chooser_model_icon_theme_changed (icon_theme, model);
      g_signal_connect (G_OBJECT (icon_theme), "changed", G_CALLBACK (exo_icon_chooser_model_icon_theme_changed), model);
    }
  else
    {
      /* take a reference for the caller */
      g_object_ref (G_OBJECT (model));
    }

  return model;
}



/**
 * _exo_icon_chooser_model_get_iter_for_icon_name:
 * @model     : an #ExoIconChooserModel.
 * @iter      : return location for the resulting #GtkTreeIter.
 * @icon_name : the name of the icon for which to lookup the iterator in the @model.
 *
 * Looks up the #GtkTreeIter for the @icon_name in the @model and returns %TRUE if the
 * @icon_name was found, %FALSE otherwise.
 *
 * Return value: %TRUE if the iterator for @icon_name was found, %FALSE otherwise.
 *
 * Since: 0.3.1.9
 **/
gboolean
_exo_icon_chooser_model_get_iter_for_icon_name (ExoIconChooserModel *model,
                                                GtkTreeIter         *iter,
                                                const gchar         *icon_name)
{
  ExoIconChooserModelItem *item;
  GList                   *lp;

  _exo_return_val_if_fail (EXO_IS_ICON_CHOOSER_MODEL (model), FALSE);
  _exo_return_val_if_fail (icon_name != NULL, FALSE);
  _exo_return_val_if_fail (iter != NULL, FALSE);

  /* check all items in the model */
  for (lp = model->items; lp != NULL; lp = lp->next)
    {
      /* compare this item's icon name */
      item = (ExoIconChooserModelItem *) lp->data;
      if (strcmp (item->icon_name, icon_name) == 0)
        {
          /* generate an iterator for this item */
          iter->stamp = model->stamp;
          iter->user_data = lp;
          return TRUE;
        }
    }

  return FALSE;
}



#define __EXO_ICON_CHOOSER_MODEL_C__
#include <exo/exo-aliasdef.c>
