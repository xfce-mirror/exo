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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <libxfce4util/libxfce4util.h>

#include <exo/exo-config.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-toolbars-editor.h>
#include <exo/exo-toolbars-private.h>
#include <exo/exo-alias.h>



#define EXO_TOOLBARS_EDITOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_TOOLBARS_EDITOR, ExoToolbarsEditorPrivate))



enum
{
  PROP_0,
  PROP_MODEL,
  PROP_UI_MANAGER,
};



static void       exo_toolbars_editor_finalize            (GObject                *object);
static void       exo_toolbars_editor_get_property        (GObject                *object,
                                                           guint                   prop_id,
                                                           GValue                 *value,
                                                           GParamSpec             *pspec);
static void       exo_toolbars_editor_set_property        (GObject                *object,
                                                           guint                   prop_id,
                                                           const GValue           *value,
                                                           GParamSpec             *pspec);
static void       exo_toolbars_editor_drag_data_get       (GtkWidget              *item,
                                                           GdkDragContext         *context,
                                                           GtkSelectionData       *selection_data,
                                                           guint                   info,
                                                           guint32                 drag_time,
                                                           ExoToolbarsEditor      *editor);
static GtkWidget *exo_toolbars_editor_create_item         (ExoToolbarsEditor      *editor,
                                                           GtkWidget              *image,
                                                           const gchar            *text,
                                                           GdkDragAction           action);
static GList*     exo_toolbars_editor_get_actions         (ExoToolbarsEditor      *editor,
                                                           ExoToolbarsModel       *model);
static void       exo_toolbars_editor_update              (ExoToolbarsEditor      *editor);



struct _ExoToolbarsEditorPrivate
{
  ExoToolbarsModel *model;
  GtkUIManager     *ui_manager;

  GtkWidget        *table;

  guint             finalizing : 1;
};



static const GtkTargetEntry targets[] =
{
  { EXO_TOOLBARS_ITEM_TYPE, GTK_TARGET_SAME_APP, 0 },
};



G_DEFINE_TYPE (ExoToolbarsEditor, exo_toolbars_editor, GTK_TYPE_VBOX)



static void
exo_toolbars_editor_class_init (ExoToolbarsEditorClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoToolbarsEditorPrivate));

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_toolbars_editor_finalize;
  gobject_class->get_property = exo_toolbars_editor_get_property;
  gobject_class->set_property = exo_toolbars_editor_set_property;

  /**
   * ExoToolbarsEditor:model:
   *
   * The #ExoToolbarsModel that should be edited from within this
   * toolbars editor. If you set this property to %NULL, the editor
   * widget will be disabled, else the editor widget will load the
   * toolbars from the given #ExoToolbarsModel and initialize its
   * user interface according to the model.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_MODEL,
                                   g_param_spec_object ("model",
                                                        "Toolbars Model",
                                                        "Toolbars Model",
                                                        EXO_TYPE_TOOLBARS_MODEL,
                                                        EXO_PARAM_READWRITE));

  /**
   * ExoToolbarsEditor:ui-manager:
   *
   * The #GtkUIManager used by this editor. If this property is
   * %NULL, the editor widget will be disabled, else if you specify
   * a valid #GtkUIManager, the editor widget will load the available
   * actions from the given user interface manager and initialize
   * its user interface according to the specified #GtkUIManager.
   *
   * The given @ui-manager needs to support all actions that were
   * specified for the model, used by the editor, with the
   * exo_toolbars_model_set_actions() method.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_UI_MANAGER,
                                   g_param_spec_object ("ui-manager",
                                                        "UI Manager",
                                                        "UI Manager",
                                                        GTK_TYPE_UI_MANAGER,
                                                        EXO_PARAM_READWRITE));
}



static void
exo_toolbars_editor_init (ExoToolbarsEditor *editor)
{
  GtkWidget *scrolled_window;
  GtkWidget *hbox;
  GtkWidget *image;
  GtkWidget *label;

  editor->priv = EXO_TOOLBARS_EDITOR_GET_PRIVATE (editor);

  g_object_set (G_OBJECT (editor),
                "border-width", 12,
                "sensitive", FALSE,
                "spacing", 6,
                NULL);

  scrolled_window = g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                                  "hscrollbar-policy", GTK_POLICY_NEVER,
                                  "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                  NULL);
  gtk_box_pack_start (GTK_BOX (editor), scrolled_window, TRUE, TRUE, 0);
  gtk_widget_show (scrolled_window);

  editor->priv->table = g_object_new (GTK_TYPE_TABLE,
                                      "border-width", 12,
                                      "column-spacing", 6,
                                      "homogeneous", TRUE,
                                      "row-spacing", 6,
                                      NULL);
  gtk_drag_dest_set (editor->priv->table, GTK_DEST_DEFAULT_ALL,
                     targets, G_N_ELEMENTS (targets), GDK_ACTION_MOVE);
  g_signal_connect_swapped (G_OBJECT (editor->priv->table), "drag-data-received",
                            G_CALLBACK (exo_toolbars_editor_update), editor);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),
                                         editor->priv->table);
  gtk_widget_show (editor->priv->table);

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (editor), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_widget_show (image);

  label = g_object_new (GTK_TYPE_LABEL,
                        "label", _("Drag an item onto the toolbars above to add it, "
                                   "from the toolbars in the items table to remove it."),
                        "wrap", TRUE,
                        "xalign", 0.0,
                        "yalign", 0.5,
                        NULL);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
  gtk_widget_show (label);
}



static void
exo_toolbars_editor_finalize (GObject *object)
{
  ExoToolbarsEditor *editor = EXO_TOOLBARS_EDITOR (object);

  editor->priv->finalizing = TRUE;

  exo_toolbars_editor_set_model (editor, NULL);
  exo_toolbars_editor_set_ui_manager (editor, NULL);

  (*G_OBJECT_CLASS (exo_toolbars_editor_parent_class)->finalize) (object);
}



static void
exo_toolbars_editor_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  ExoToolbarsEditor *editor = EXO_TOOLBARS_EDITOR (object);

  switch (prop_id)
    {
    case PROP_MODEL:
      g_value_set_object (value, editor->priv->model);
      break;

    case PROP_UI_MANAGER:
      g_value_set_object (value, editor->priv->ui_manager);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_toolbars_editor_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  ExoToolbarsEditor *editor = EXO_TOOLBARS_EDITOR (object);

  switch (prop_id)
    {
    case PROP_MODEL:
      exo_toolbars_editor_set_model (editor, g_value_get_object (value));
      break;

    case PROP_UI_MANAGER:
      exo_toolbars_editor_set_ui_manager (editor, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_toolbars_editor_drag_data_get (GtkWidget          *item,
                                   GdkDragContext     *context,
                                   GtkSelectionData   *selection_data,
                                   guint               info,
                                   guint32             drag_time,
                                   ExoToolbarsEditor  *editor)
{
  const gchar *target;
  GtkAction   *action;

  action = g_object_get_data (G_OBJECT (item), I_("gtk-action"));
  target = (action != NULL) ? gtk_action_get_name (action) : "separator";
  gtk_selection_data_set (selection_data, selection_data->target,
                          8, (const guchar *) target, strlen (target));
}



static void
event_box_realize (GtkWidget *widget,
                   GtkImage  *image)
{
  GtkImageType type;
  GdkPixbuf   *pixbuf;
  gchar       *stock_id;

  _exo_toolbars_set_drag_cursor (widget);

  type = gtk_image_get_storage_type (image);
  if (type == GTK_IMAGE_STOCK)
    {
      gtk_image_get_stock (image, &stock_id, NULL);
      pixbuf = gtk_widget_render_icon (widget, stock_id, GTK_ICON_SIZE_LARGE_TOOLBAR, NULL);
      gtk_drag_source_set_icon_pixbuf (widget, pixbuf);
    }
  else if (type == GTK_IMAGE_PIXBUF)
    {
      pixbuf = gtk_image_get_pixbuf (image);
      gtk_drag_source_set_icon_pixbuf (widget, pixbuf);
    }
}



static GtkWidget*
exo_toolbars_editor_create_item (ExoToolbarsEditor  *editor,
                                 GtkWidget          *image,
                                 const gchar        *text,
                                 GdkDragAction       action)
{
  GtkWidget   *ebox;
  GtkWidget   *vbox;
  GtkWidget   *label;
  gchar       *text_no_mnemonic;

  ebox = gtk_event_box_new ();
  gtk_drag_source_set (ebox, GDK_BUTTON1_MASK,
                       targets, G_N_ELEMENTS (targets),
                       action);
  g_signal_connect (G_OBJECT (ebox), "drag-data-get",
                    G_CALLBACK (exo_toolbars_editor_drag_data_get), editor);
  g_signal_connect_after (G_OBJECT (ebox), "realize",
                          G_CALLBACK (event_box_realize), image);
  g_signal_connect_swapped (G_OBJECT (ebox), "drag-data-delete",
                            G_CALLBACK (exo_toolbars_editor_update), editor);
  gtk_widget_show (ebox);

  if (action == GDK_ACTION_MOVE)
    {
      g_signal_connect (G_OBJECT (ebox), "drag-begin",
                        G_CALLBACK (gtk_widget_hide), NULL);
      g_signal_connect (G_OBJECT (ebox), "drag-end",
                        G_CALLBACK (gtk_widget_show), NULL);
    }

  vbox = gtk_vbox_new (0, FALSE);
  gtk_container_add (GTK_CONTAINER (ebox), vbox);
  gtk_widget_show (vbox);

  gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, TRUE, 0);
  gtk_widget_show (image);

  text_no_mnemonic = exo_str_elide_underscores (text);
  label = gtk_label_new (text_no_mnemonic);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);
  g_free (text_no_mnemonic);
  gtk_widget_show (label);

  return ebox;
}



static gboolean
model_has_action (ExoToolbarsModel *model,
                  GtkAction        *action)
{
  const gchar *action_name;
  const gchar *id;
  gboolean     is_separator;
  gint         i;
  gint         j;

  action_name = gtk_action_get_name (action);

  for (i = 0; i < exo_toolbars_model_n_toolbars (model); ++i)
    for (j = 0; j < exo_toolbars_model_n_items (model, i); ++j)
      {
        exo_toolbars_model_item_nth (model, i, j, &is_separator, &id, NULL);
        if (!is_separator && exo_str_is_equal (id, action_name))
          return TRUE;
      }

  return FALSE;
}



static gboolean
compare_actions (gconstpointer a,
                 gconstpointer b)
{
  gchar *label_a;
  gchar *label_b;
  gint   result;

  g_object_get (G_OBJECT (a), "short-label", &label_a, NULL);
  g_object_get (G_OBJECT (b), "short-label", &label_b, NULL);

  result = g_utf8_collate (label_a, label_b);

  g_free (label_b);
  g_free (label_a);

  return result;
}



static GList*
exo_toolbars_editor_get_actions (ExoToolbarsEditor *editor,
                                 ExoToolbarsModel  *model)
{
  GtkAction *action;
  gchar    **actions;
  GList     *result = NULL;
  guint      n;

  actions = exo_toolbars_model_get_actions (model);
  if (G_LIKELY (actions != NULL))
    {
      for (n = 0; actions[n] != NULL; ++n)
        {
          action = _exo_toolbars_find_action (editor->priv->ui_manager, actions[n]);
          if (G_UNLIKELY (action == NULL))
            continue;

          if (!model_has_action (model, action))
            result = g_list_insert_sorted (result, action, compare_actions);
        }

      g_strfreev (actions);
    }

  return result;
}



static void
exo_toolbars_editor_update (ExoToolbarsEditor *editor)
{
  GtkAction         *action;
  GtkWidget         *image;
  GtkWidget         *item;
  GList             *children;
  GList             *actions;
  GList             *lp;
  gchar             *stock;
  gchar             *text;
  gint               height;
  gint               width = 4;
  gint               x = 0;
  gint               y = 0;

  if (editor->priv->finalizing)
    return;

  /* remove all existing tool items */
  children = gtk_container_get_children (GTK_CONTAINER (editor->priv->table));
  for (lp = children; lp != NULL; lp = lp->next)
    gtk_container_remove (GTK_CONTAINER (editor->priv->table), lp->data);
  g_list_free (children);

  if (editor->priv->model == NULL || editor->priv->ui_manager == NULL)
    return;

  gtk_widget_set_sensitive (GTK_WIDGET (editor), TRUE);

  actions = exo_toolbars_editor_get_actions (editor, editor->priv->model);
  height = g_list_length (actions) / width + 1;
  gtk_table_resize (GTK_TABLE (editor->priv->table), height, width);
  for (lp = actions; lp != NULL; lp = lp->next)
    {
      action = lp->data;

      g_object_get (G_OBJECT (action),
                    "short-label", &text,
                    "stock-id", &stock,
                    NULL);

      if (G_UNLIKELY (stock == NULL))
        stock = g_strdup (GTK_STOCK_DND);

      image = gtk_image_new_from_stock (stock, GTK_ICON_SIZE_LARGE_TOOLBAR);
      item = exo_toolbars_editor_create_item (editor, image, text,
                                              GDK_ACTION_MOVE);
      g_object_set_data (G_OBJECT (item), I_("gtk-action"), action);
      gtk_table_attach_defaults (GTK_TABLE (editor->priv->table),
                                 item, x, x + 1, y, y + 1);

      g_free (stock);
      g_free (text);

      if (++x >= width)
        {
          x= 0; ++y;
        }
    }
  g_list_free (actions);

  image = _exo_toolbars_new_separator_image ();
  item = exo_toolbars_editor_create_item (editor, image,
                                          _("Separator"),
                                          GDK_ACTION_COPY);
  gtk_table_attach_defaults (GTK_TABLE (editor->priv->table),
                             item, x, x + 1, y, y + 1);

  return;
}



/**
 * exo_toolbars_editor_new:
 * @ui_manager  : A #GtkUIManager.
 *
 * Creates a new #ExoToolbarsEditor that will
 * be associated with @ui_manager. @ui_manager must
 * be a valid #GtkUIManager, %NULL is not allowed
 * at this point.
 *
 * The newly created #ExoToolbarsEditor will not
 * be usable until you associate an #ExoToolbarsModel
 * with it, using the function exo_toolbars_editor_set_model().
 * You should probably use exo_toolbars_editor_new_with_model()
 * instead.
 *
 * Return value: A new #ExoToolbarsEditor.
 **/
GtkWidget*
exo_toolbars_editor_new (GtkUIManager *ui_manager)
{
  g_return_val_if_fail (GTK_IS_UI_MANAGER (ui_manager), NULL);

  return g_object_new (EXO_TYPE_TOOLBARS_EDITOR,
                       "ui-manager", ui_manager,
                       NULL);
}



/**
 * exo_toolbars_editor_new_with_model:
 * @ui_manager  : A #GtkUIManager.
 * @model       : An #ExoToolbarsModel.
 *
 * Creates a new #ExoToolbarsEditor that will be
 * associated with @model and @ui_manager. You
 * must supply a valid #GtkUIManager and a valid
 * #ExoToolbarsModel here or the function will
 * fail.
 *
 * Return value: A new #ExoToolbarsEditor.
 **/
GtkWidget*
exo_toolbars_editor_new_with_model (GtkUIManager     *ui_manager,
                                    ExoToolbarsModel *model)
{
  g_return_val_if_fail (GTK_IS_UI_MANAGER (ui_manager), NULL);
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), NULL);

  return g_object_new (EXO_TYPE_TOOLBARS_EDITOR,
                       "ui-manager", ui_manager,
                       "model", model,
                       NULL);
}



/**
 * exo_toolbars_editor_get_model:
 * @editor  : An #ExoToolbarsEditor.
 *
 * Returns the #ExoToolbarsModel currently associated
 * with @editor or %NULL if no #ExoToolbarsModel is
 * currently associated with @editor.
 *
 * Return value: An #ExoToolbarsModel or %NULL.
 **/
ExoToolbarsModel*
exo_toolbars_editor_get_model (ExoToolbarsEditor *editor)
{
  g_return_val_if_fail (EXO_IS_TOOLBARS_EDITOR (editor), NULL);
  return editor->priv->model;
}



/**
 * exo_toolbars_editor_set_model:
 * @editor  : An #ExoToolbarsEditor.
 * @model   : An #ExoToolbarsModel or %NULL.
 *
 * Sets the model to edit by this @editor. If you specify
 * %NULL for @model, the editor widget will be disabled.
 * Else the editor widget will load the toolbars from
 * @model and reinitialize its user interface according
 * to @model.
 **/
void
exo_toolbars_editor_set_model (ExoToolbarsEditor *editor,
                               ExoToolbarsModel  *model)
{
  g_return_if_fail (EXO_IS_TOOLBARS_EDITOR (editor));
  g_return_if_fail (EXO_IS_TOOLBARS_MODEL (model)
                    || model == NULL);

  if (G_UNLIKELY (editor->priv->model == model))
    return;

  if (editor->priv->model != NULL)
    {
      g_signal_handlers_disconnect_by_func (G_OBJECT (editor->priv->model),
                                            exo_toolbars_editor_update,
                                            editor);

      g_object_unref (G_OBJECT (editor->priv->model));
    }

  editor->priv->model = model;

  if (model != NULL)
    {
      g_object_ref (G_OBJECT (model));

      g_signal_connect_swapped (G_OBJECT (model), "item-added",
                                G_CALLBACK (exo_toolbars_editor_update), editor);
      g_signal_connect_swapped (G_OBJECT (model), "item-removed",
                                G_CALLBACK (exo_toolbars_editor_update), editor);
      g_signal_connect_swapped (G_OBJECT (model), "toolbar-added",
                                G_CALLBACK (exo_toolbars_editor_update), editor);
      g_signal_connect_swapped (G_OBJECT (model), "toolbar-removed",
                                G_CALLBACK (exo_toolbars_editor_update), editor);
    }

  exo_toolbars_editor_update (editor);

  g_object_notify (G_OBJECT (editor), "model");
}



/**
 * exo_toolbars_editor_get_ui_manager:
 * @editor : An #ExoToolbarsEditor.
 *
 * Returns the #GtkUIManager associated with
 * @editor or %NULL if no user interface
 * manager is associated with @editor currently.
 *
 * Return value: A #GtkUIManager or %NULL.
 **/
GtkUIManager*
exo_toolbars_editor_get_ui_manager (ExoToolbarsEditor *editor)
{
  g_return_val_if_fail (EXO_IS_TOOLBARS_EDITOR (editor), NULL);
  return editor->priv->ui_manager;
}



/**
 * exo_toolbars_editor_set_ui_manager:
 * @editor      : An #ExoToolbarsEditor.
 * @ui_manager  : A #GtkUIManager or %NULL.
 *
 * Sets the #GtkUIManager to use by this #ExoToolbarsEditor. If you
 * specify %NULL for @ui_manager, the editor widget will be disabled.
 * Else the editor will load the available actions from @ui_manager
 * and reinitialize the user interface.
 **/
void
exo_toolbars_editor_set_ui_manager (ExoToolbarsEditor *editor,
                                    GtkUIManager      *ui_manager)
{
  g_return_if_fail (EXO_IS_TOOLBARS_EDITOR (editor));
  g_return_if_fail (GTK_IS_UI_MANAGER (ui_manager) || ui_manager == NULL);

  if (G_UNLIKELY (editor->priv->ui_manager == ui_manager))
    return;

  if (editor->priv->ui_manager != NULL)
    g_object_unref (G_OBJECT (editor->priv->ui_manager));

  editor->priv->ui_manager = ui_manager;

  if (ui_manager != NULL)
    g_object_ref (G_OBJECT (ui_manager));

  exo_toolbars_editor_update (editor);
}



#define __EXO_TOOLBARS_EDITOR_C__
#include <exo/exo-aliasdef.c>
