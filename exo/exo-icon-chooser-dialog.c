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
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <exo/exo-binding.h>
#include <exo/exo-cell-renderer-ellipsized-text.h>
#include <exo/exo-cell-renderer-icon.h>
#include <exo/exo-gtk-extensions.h>
#include <exo/exo-icon-chooser-dialog.h>
#include <exo/exo-icon-chooser-model.h>
#include <exo/exo-icon-view.h>
#include <exo/exo-private.h>
#include <exo/exo-alias.h>

/**
 * SECTION: exo-icon-chooser-dialog
 * @title: ExoIconChooserDialog
 * @short_description: Dialog to select icons
 * @include: exo/exo.h
 * @see_also: <ulink type="http" href="http://library.gnome.org/devel/gtk/stable/GtkIconTheme.html">
 *            GtkIconTheme</ulink>
 *
 * The #ExoIconChooserDialog class provides an easy to use dialog to ask
 * the user to select either a named icon from the selected icon theme,
 * or an image file from the local file system.
 **/



#define EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
    EXO_TYPE_ICON_CHOOSER_DIALOG, ExoIconChooserDialogPrivate))



static void     exo_icon_chooser_dialog_style_set         (GtkWidget                  *widget,
                                                           GtkStyle                   *previous_style);
static void     exo_icon_chooser_dialog_screen_changed    (GtkWidget                  *widget,
                                                           GdkScreen                  *previous_screen);
static void     exo_icon_chooser_dialog_close             (GtkDialog                  *dialog);
static gboolean exo_icon_chooser_dialog_separator_func    (GtkTreeModel               *model,
                                                           GtkTreeIter                *iter,
                                                           gpointer                    user_data);
static gboolean exo_icon_chooser_dialog_visible_func      (GtkTreeModel               *model,
                                                           GtkTreeIter                *iter,
                                                           gpointer                    user_data);
static void     exo_icon_chooser_dialog_combo_changed     (GtkWidget                  *combo,
                                                           ExoIconChooserDialog       *icon_chooser_dialog);
static void     exo_icon_chooser_dialog_selection_changed (ExoIconChooserDialog       *icon_chooser_dialog);



struct _ExoIconChooserDialogPrivate
{
  GtkWidget *combo;
  GtkWidget *icon_chooser;
  GtkWidget *file_chooser;
  GtkWidget *file_preview;
};



static const gchar CONTEXT_TITLES[][28] =
{
  /* EXO_ICON_CHOOSER_CONTEXT_ACTIONS */
  N_("Action Icons"),
  /* EXO_ICON_CHOOSER_CONTEXT_ANIMATIONS */
  N_("Animations"),
  /* EXO_ICON_CHOOSER_CONTEXT_APPLICATIONS */
  N_("Application Icons"),
  /* EXO_ICON_CHOOSER_CONTEXT_CATEGORIES */
  N_("Menu Icons"),
  /* EXO_ICON_CHOOSER_CONTEXT_DEVICES */
  N_("Device Icons"),
  /* EXO_ICON_CHOOSER_CONTEXT_EMBLEMS */
  N_("Emblems"),
  /* EXO_ICON_CHOOSER_CONTEXT_EMOTES */
  N_("Emoticons"),
  /* EXO_ICON_CHOOSER_CONTEXT_INTERNATIONAL */
  N_("International Denominations"),
  /* EXO_ICON_CHOOSER_CONTEXT_MIME_TYPES */
  N_("File Type Icons"),
  /* EXO_ICON_CHOOSER_CONTEXT_PLACES */
  N_("Location Icons"),
  /* EXO_ICON_CHOOSER_CONTEXT_STATUS */
  N_("Status Icons"),
  /* EXO_ICON_CHOOSER_CONTEXT_OTHER */
  N_("Uncategorized Icons"),
  /* separator */
  "#",
  /* EXO_ICON_CHOOSER_CONTEXT_ALL */
  N_("All Icons"),
  /* separator */
  "#",
  /* EXO_ICON_CHOOSER_CONTEXT_FILE */
  N_("Image Files"),
};



G_DEFINE_TYPE (ExoIconChooserDialog, exo_icon_chooser_dialog, GTK_TYPE_DIALOG)



static void
exo_icon_chooser_dialog_class_init (ExoIconChooserDialogClass *klass)
{
  GtkDialogClass *gtkdialog_class;
  GtkWidgetClass *gtkwidget_class;
  GtkBindingSet  *binding_set;

  /* add our private data to the type's instances */
  g_type_class_add_private (klass, sizeof (ExoIconChooserDialogPrivate));

  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->style_set = exo_icon_chooser_dialog_style_set;
  gtkwidget_class->screen_changed = exo_icon_chooser_dialog_screen_changed;

  gtkdialog_class = GTK_DIALOG_CLASS (klass);
  gtkdialog_class->close = exo_icon_chooser_dialog_close;

  /* connect additional key bindings to the GtkDialog::close action signal */
  binding_set = gtk_binding_set_by_class (klass);
  gtk_binding_entry_add_signal (binding_set, GDK_w, GDK_CONTROL_MASK, "close", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_W, GDK_CONTROL_MASK, "close", 0);
}



static void
exo_icon_chooser_dialog_init (ExoIconChooserDialog *icon_chooser_dialog)
{
  ExoIconChooserDialogPrivate *priv = EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE (icon_chooser_dialog);
  ExoIconChooserContext        context;
  ExoIconChooserModel         *chooser_model;
  GtkCellRenderer             *renderer;
  GtkFileFilter               *filter;
  GtkTreeModel                *filter_model;
  GtkWidget                   *scrolled_window;
  GtkWidget                   *label;
  GtkWidget                   *hbox;
  GtkWidget                   *vbox;

  /* initialize the library's i18n support */
  _exo_i18n_init ();

  gtk_window_set_default_size (GTK_WINDOW (icon_chooser_dialog), 780, 560);

  gtk_widget_push_composite_child ();

  /* add the main box */
  vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (icon_chooser_dialog)->vbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);

  /* add the header box */
  hbox = gtk_hbox_new (FALSE, 12);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  /* setup the header label */
  label = gtk_label_new_with_mnemonic (_("Select _icon from:"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  /* setup the context combo box */
  priv->combo = gtk_combo_box_new_text ();
  for (context = 0; context < G_N_ELEMENTS (CONTEXT_TITLES); ++context)
    gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo), _(CONTEXT_TITLES[context]));
  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (priv->combo), exo_icon_chooser_dialog_separator_func, icon_chooser_dialog, NULL);
  g_signal_connect (G_OBJECT (priv->combo), "changed", G_CALLBACK (exo_icon_chooser_dialog_combo_changed), icon_chooser_dialog);
  gtk_box_pack_start (GTK_BOX (hbox), priv->combo, TRUE, TRUE, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), priv->combo);
  gtk_widget_show (priv->combo);

  /* setup the scrolled window for the icon chooser */
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_window), GTK_SHADOW_ETCHED_IN);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
  gtk_widget_show (scrolled_window);

  /* setup the icon chooser (shown by default) */
  priv->icon_chooser = exo_icon_view_new ();
  exo_binding_new (G_OBJECT (priv->icon_chooser), "visible", G_OBJECT (scrolled_window), "visible");
  g_signal_connect_swapped (priv->icon_chooser, "item-activated", G_CALLBACK (gtk_window_activate_default), icon_chooser_dialog);
  g_signal_connect_swapped (priv->icon_chooser, "selection-changed", G_CALLBACK (exo_icon_chooser_dialog_selection_changed), icon_chooser_dialog);
  gtk_container_add (GTK_CONTAINER (scrolled_window), priv->icon_chooser);
  gtk_widget_show (priv->icon_chooser);

  /* setup the icon renderer */
  renderer = exo_cell_renderer_icon_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (priv->icon_chooser), renderer, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (priv->icon_chooser), renderer, "icon", EXO_ICON_CHOOSER_MODEL_COLUMN_ICON_NAME, NULL);

  /* setup the text renderer */
  renderer = g_object_new (EXO_TYPE_CELL_RENDERER_ELLIPSIZED_TEXT,
                           "follow-state", TRUE,
                           "wrap-mode", PANGO_WRAP_WORD_CHAR,
                           "wrap-width", 104,
                           "xalign", 0.5f,
                           "yalign", 0.0f,
                           NULL);
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (priv->icon_chooser), renderer, FALSE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (priv->icon_chooser), renderer, "text", EXO_ICON_CHOOSER_MODEL_COLUMN_DISPLAY_NAME, NULL);

  /* setup the file chooser (hidden by default) */
  priv->file_chooser = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (priv->file_chooser), TRUE);
  exo_gtk_file_chooser_add_thumbnail_preview (GTK_FILE_CHOOSER (priv->file_chooser));
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (priv->file_chooser), DATADIR G_DIR_SEPARATOR_S "pixmaps");
  g_signal_connect_swapped (priv->file_chooser, "file-activated", G_CALLBACK (gtk_window_activate_default), icon_chooser_dialog);
  g_signal_connect_swapped (priv->file_chooser, "selection-changed", G_CALLBACK (exo_icon_chooser_dialog_selection_changed), icon_chooser_dialog);
  gtk_box_pack_start (GTK_BOX (vbox), priv->file_chooser, TRUE, TRUE, 0);

  /* add file chooser filters */
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Image Files"));
  gtk_file_filter_add_pixbuf_formats (filter);
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (priv->file_chooser), filter);
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (priv->file_chooser), filter);

  /* setup the filtered icon model for the icon chooser */
  chooser_model = _exo_icon_chooser_model_get_for_widget (GTK_WIDGET (icon_chooser_dialog));
  filter_model = gtk_tree_model_filter_new (GTK_TREE_MODEL (chooser_model), NULL);
  gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (filter_model), exo_icon_chooser_dialog_visible_func, icon_chooser_dialog, NULL);
  exo_icon_view_set_model (EXO_ICON_VIEW (priv->icon_chooser), filter_model);
  g_object_unref (G_OBJECT (chooser_model));
  g_object_unref (G_OBJECT (filter_model));

  /* enable search on the display name */
  exo_icon_view_set_search_column (EXO_ICON_VIEW (priv->icon_chooser), EXO_ICON_CHOOSER_MODEL_COLUMN_DISPLAY_NAME);

  /* default to "Application Icons", since thats what users probably expect to see */
  gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combo), EXO_ICON_CHOOSER_CONTEXT_APPLICATIONS);

  gtk_widget_pop_composite_child ();
}



static void
exo_icon_chooser_dialog_style_set (GtkWidget *widget,
                                   GtkStyle  *previous_style)
{
  ExoIconChooserDialogPrivate *priv = EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE (widget);
  ExoIconChooserModel         *model;
  GtkTreeModel                *filter;

  /* call the parent's style_set method */
  if (GTK_WIDGET_CLASS (exo_icon_chooser_dialog_parent_class)->style_set != NULL)
    (*GTK_WIDGET_CLASS (exo_icon_chooser_dialog_parent_class)->style_set) (widget, previous_style);

  /* determine the icon chooser model for the widget */
  model = _exo_icon_chooser_model_get_for_widget (widget);

  /* check if we have a new model here */
  filter = exo_icon_view_get_model (EXO_ICON_VIEW (priv->icon_chooser));
  if (GTK_TREE_MODEL (model) != gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (filter)))
    {
      /* setup a new filter for the model */
      filter = gtk_tree_model_filter_new (GTK_TREE_MODEL (model), NULL);
      gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (filter), exo_icon_chooser_dialog_visible_func, widget, NULL);
      exo_icon_view_set_model (EXO_ICON_VIEW (priv->icon_chooser), filter);
      g_object_unref (G_OBJECT (filter));

      /* enable search on the display name */
      exo_icon_view_set_search_column (EXO_ICON_VIEW (priv->icon_chooser), EXO_ICON_CHOOSER_MODEL_COLUMN_DISPLAY_NAME);
    }
  g_object_unref (G_OBJECT (model));
}



static void
exo_icon_chooser_dialog_screen_changed (GtkWidget *widget,
                                        GdkScreen *previous_screen)
{
  ExoIconChooserDialogPrivate *priv = EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE (widget);
  ExoIconChooserModel         *model;
  GtkTreeModel                *filter;

  /* call the parent's screen_changed method */
  if (GTK_WIDGET_CLASS (exo_icon_chooser_dialog_parent_class)->screen_changed != NULL)
    (*GTK_WIDGET_CLASS (exo_icon_chooser_dialog_parent_class)->screen_changed) (widget, previous_screen);

  /* determine the icon chooser model for the widget */
  model = _exo_icon_chooser_model_get_for_widget (widget);

  /* check if we have a new model here */
  filter = exo_icon_view_get_model (EXO_ICON_VIEW (priv->icon_chooser));
  if (GTK_TREE_MODEL (model) != gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (filter)))
    {
      /* setup a new filter for the model */
      filter = gtk_tree_model_filter_new (GTK_TREE_MODEL (model), NULL);
      gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (filter), exo_icon_chooser_dialog_visible_func, widget, NULL);
      exo_icon_view_set_model (EXO_ICON_VIEW (priv->icon_chooser), filter);
      g_object_unref (G_OBJECT (filter));

      /* enable search on the display name */
      exo_icon_view_set_search_column (EXO_ICON_VIEW (priv->icon_chooser), EXO_ICON_CHOOSER_MODEL_COLUMN_DISPLAY_NAME);
    }
  g_object_unref (G_OBJECT (model));
}



static void
exo_icon_chooser_dialog_close (GtkDialog *dialog)
{
  GdkEvent *event;

  /* verify that the dialog is realized */
  if (G_LIKELY (GTK_WIDGET_REALIZED (dialog)))
    {
      /* send a delete event to the dialog */
      event = gdk_event_new (GDK_DELETE);
      event->any.window = g_object_ref (GTK_WIDGET (dialog)->window);
      event->any.send_event = TRUE;
      gtk_main_do_event (event);
      gdk_event_free (event);
    }
}



static gboolean
exo_icon_chooser_dialog_separator_func (GtkTreeModel *model,
                                        GtkTreeIter  *iter,
                                        gpointer      user_data)
{
  gboolean separator;
  gchar   *title;

  /* check if we have a separator here */
  gtk_tree_model_get (model, iter, 0, &title, -1);
  separator = (title[0] == '#' && title[1] == '\0');
  g_free (title);

  return separator;
}



static gboolean
exo_icon_chooser_dialog_visible_func (GtkTreeModel *model,
                                      GtkTreeIter  *iter,
                                      gpointer      user_data)
{
  ExoIconChooserDialogPrivate *priv = EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE (user_data);
  guint                        icon_chooser_context;
  guint                        item_context;

  /* check if we need to test the context */
  icon_chooser_context = gtk_combo_box_get_active (GTK_COMBO_BOX (priv->combo));
  if (G_LIKELY (icon_chooser_context < EXO_ICON_CHOOSER_CONTEXT_ALL))
    {
      /* determine the context for the iter... */
      gtk_tree_model_get (model, iter, EXO_ICON_CHOOSER_MODEL_COLUMN_CONTEXT, &item_context, -1);

      /* ...and compare them */
      return (icon_chooser_context == item_context);
    }

  /* all icons should be shown */
  return TRUE;
}



static void
exo_icon_chooser_dialog_combo_changed (GtkWidget            *combo,
                                       ExoIconChooserDialog *icon_chooser_dialog)
{
  ExoIconChooserDialogPrivate *priv = EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE (icon_chooser_dialog);
  ExoIconChooserContext        context;
  GtkTreeModel                *model;
  GList                       *selected_items;

  /* determine the new active context */
  context = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (context <= EXO_ICON_CHOOSER_CONTEXT_ALL)
    {
      /* hide the file chooser/show the icon chooser */
      gtk_widget_hide (priv->file_chooser);
      gtk_widget_show (priv->icon_chooser);

      /* need to re-filter with the new context */
      model = exo_icon_view_get_model (EXO_ICON_VIEW (priv->icon_chooser));
      if (G_LIKELY (model != NULL))
        gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (model));

      /* check if the icon chooser has a selected item */
      selected_items = exo_icon_view_get_selected_items (EXO_ICON_VIEW (priv->icon_chooser));
      if (G_LIKELY (selected_items != NULL))
        {
          /* make sure the selected item is visible */
          exo_icon_view_scroll_to_path (EXO_ICON_VIEW (priv->icon_chooser), selected_items->data, FALSE, 0.0f, 0.0f);
          g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
          g_list_free (selected_items);
        }
    }
  else
    {
      /* show the file chooser/hide the icon chooser */
      gtk_widget_show (priv->file_chooser);
      gtk_widget_hide (priv->icon_chooser);
    }

  /* we certainly changed the selection this way */
  exo_icon_chooser_dialog_selection_changed (icon_chooser_dialog);
}



static void
exo_icon_chooser_dialog_selection_changed (ExoIconChooserDialog *icon_chooser_dialog)
{
  gchar *icon;

  /* check if we have a valid icon in the chooser */
  icon = exo_icon_chooser_dialog_get_icon (icon_chooser_dialog);

  /* update all response-ids that we recognize */
  gtk_dialog_set_response_sensitive (GTK_DIALOG (icon_chooser_dialog), GTK_RESPONSE_ACCEPT, (icon != NULL));
  gtk_dialog_set_response_sensitive (GTK_DIALOG (icon_chooser_dialog), GTK_RESPONSE_APPLY, (icon != NULL));
  gtk_dialog_set_response_sensitive (GTK_DIALOG (icon_chooser_dialog), GTK_RESPONSE_OK, (icon != NULL));
  gtk_dialog_set_response_sensitive (GTK_DIALOG (icon_chooser_dialog), GTK_RESPONSE_YES, (icon != NULL));

  /* cleanup */
  g_free (icon);
}



/**
 * exo_icon_chooser_dialog_new:
 * @title             : title of the dialog, or %NULL.
 * @parent            : transient parent of the dialog, or %NULL.
 * @first_button_text : stock-id or text to go in the first button, or %NULL.
 * @...               : response-id for the first button, then additional (button, id) pairs,
 *                      ending with %NULL.
 *
 * Creates a new #ExoIconChooserDialog. This function is analogous to gtk_dialog_new_with_buttons().
 *
 * Returns: a new #ExoIconChooserDialog.
 *
 * Since: 0.3.1.9
 **/
GtkWidget*
exo_icon_chooser_dialog_new (const gchar *title,
                             GtkWindow   *parent,
                             const gchar *first_button_text,
                             ...)
{
  const gchar *button_text;
  GtkWidget   *dialog;
  va_list      var_args;

  g_return_val_if_fail (parent == NULL || GTK_IS_WINDOW (parent), NULL);

  dialog = g_object_new (EXO_TYPE_ICON_CHOOSER_DIALOG,
                         "has-separator", FALSE,
                         "title", title,
                         NULL);

  if (G_LIKELY (parent != NULL))
    {
      gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);
      gtk_window_set_modal (GTK_WINDOW (dialog), gtk_window_get_modal (parent));
      gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);
    }

  va_start (var_args, first_button_text);
  for (button_text = first_button_text; button_text != NULL; button_text = va_arg (var_args, const gchar *))
    gtk_dialog_add_button (GTK_DIALOG (dialog), button_text, va_arg (var_args, gint));
  va_end (var_args);

  return dialog;
}



/**
 * exo_icon_chooser_dialog_get_icon:
 * @icon_chooser_dialog : an #ExoIconChooserDialog.
 *
 * Returns the currently selected icon for the @icon_chooser_dialog. The selected icon can be either
 * a named icon (from the active icon theme) or the absolute path to an image file in the file system.
 * You can distinguish between those two icon types using the g_path_is_absolute() function. If no
 * icon is currently selected, %NULL will be returned.
 *
 * The caller is responsible to free the returned string using g_free() when no longer needed.
 *
 * Returns: the currently selected icon for @icon_chooser_dialog or %NULL if no icon is selected.
 *
 * Since: 0.3.1.9
 **/
gchar*
exo_icon_chooser_dialog_get_icon (ExoIconChooserDialog *icon_chooser_dialog)
{
  ExoIconChooserDialogPrivate *priv = EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE (icon_chooser_dialog);
  GtkTreeModel                *model;
  GtkTreeIter                  iter;
  GList                       *selected_items;
  gchar                       *icon = NULL;

  g_return_val_if_fail (EXO_IS_ICON_CHOOSER_DIALOG (icon_chooser_dialog), NULL);

  /* determine the active context for the chooser */
  if (gtk_combo_box_get_active (GTK_COMBO_BOX (priv->combo)) <= EXO_ICON_CHOOSER_CONTEXT_ALL)
    {
      /* user is selecting a named icon, check if atleast one selected */
      selected_items = exo_icon_view_get_selected_items (EXO_ICON_VIEW (priv->icon_chooser));
      if (G_LIKELY (selected_items != NULL))
        {
          /* determine the name of the first selected icon */
          model = exo_icon_view_get_model (EXO_ICON_VIEW (priv->icon_chooser));
          if (gtk_tree_model_get_iter (model, &iter, selected_items->data))
            gtk_tree_model_get (model, &iter, EXO_ICON_CHOOSER_MODEL_COLUMN_ICON_NAME, &icon, -1);

          /* release the list of selected items */
          g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
          g_list_free (selected_items);
        }
    }
  else
    {
      /* user is selecting an image file, so just return the absolute path to the image file */
      icon = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (priv->file_chooser));
      if (icon != NULL && gdk_pixbuf_get_file_info (icon, NULL, NULL) == NULL)
        {
          /* whatever the user selected, it's not an icon that we support */
          g_free (icon);
          icon = NULL;
        }
    }

  return icon;
}



/**
 * exo_icon_chooser_dialog_set_icon:
 * @icon_chooser_dialog : an #ExoIconChooserDialog.
 * @icon                : the themed icon or the absolute path to an image file to select.
 *
 * Preselects the specified @icon in the @icon_chooser_dialog, and returns %TRUE if the
 * @icon was successfully selected.
 *
 * Returns: %TRUE if the @icon was successfully preselected in the @icon_chooser_dialog,
 *          %FALSE otherwise.
 *
 * Since: 0.3.1.9
 **/
gboolean
exo_icon_chooser_dialog_set_icon (ExoIconChooserDialog *icon_chooser_dialog,
                                  const gchar          *icon)
{
  ExoIconChooserDialogPrivate *priv = EXO_ICON_CHOOSER_DIALOG_GET_PRIVATE (icon_chooser_dialog);
  GtkTreeModel                *filter;
  GtkTreeModel                *model;
  GtkTreePath                 *filter_path;
  GtkTreePath                 *model_path;
  GtkTreeIter                  model_iter;
  guint                        context;

  g_return_val_if_fail (EXO_IS_ICON_CHOOSER_DIALOG (icon_chooser_dialog), FALSE);
  g_return_val_if_fail (icon != NULL, FALSE);

  /* check if we have a file or a named icon here */
  if (g_path_is_absolute (icon))
    {
      /* try to select the filename in the file chooser */
      if (gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (priv->file_chooser), icon))
        {
          /* need to display the file chooser with the newly selected filename */
          gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combo), EXO_ICON_CHOOSER_CONTEXT_FILE);
          return TRUE;
        }
    }
  else
    {
      /* determine the real model and the filter for the model */
      filter = exo_icon_view_get_model (EXO_ICON_VIEW (priv->icon_chooser));
      model = gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (filter));

      /* lookup the named icon in the model */
      if (_exo_icon_chooser_model_get_iter_for_icon_name (EXO_ICON_CHOOSER_MODEL (model), &model_iter, icon))
        {
          /* determine the path for the iterator in the real model */
          model_path = gtk_tree_model_get_path (model, &model_iter);
          if (G_LIKELY (model_path != NULL))
            {
              /* translate the path in the real model to a path in the filter */
              filter_path = gtk_tree_model_filter_convert_child_path_to_path (GTK_TREE_MODEL_FILTER (filter), model_path);
              if (G_UNLIKELY (filter_path == NULL))
                {
                  /* determine the context for the iterator in the real model */
                  gtk_tree_model_get (model, &model_iter, EXO_ICON_CHOOSER_MODEL_COLUMN_CONTEXT, &context, -1);

                  /* switch the filter to the context for the item at the real iter */
                  gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combo), context);

                  /* now we should be able to determine the filter path */
                  filter_path = gtk_tree_model_filter_convert_child_path_to_path (GTK_TREE_MODEL_FILTER (filter), model_path);
                }

              /* check if we have a valid path in the filter */
              if (G_LIKELY (filter_path != NULL))
                {
                  /* select the icon and place the cursor on the newly selected icon */
                  exo_icon_view_select_path (EXO_ICON_VIEW (priv->icon_chooser), filter_path);
                  exo_icon_view_set_cursor (EXO_ICON_VIEW (priv->icon_chooser), filter_path, NULL, FALSE);
                  gtk_tree_path_free (filter_path);
                }

              /* release the path in the real model */
              gtk_tree_path_free (model_path);

              /* the icon was successfully selected if we have a filter path */
              return (filter_path != NULL);
            }
        }
    }

  return FALSE;
}



#define __EXO_ICON_CHOOSER_DIALOG_C__
#include <exo/exo-aliasdef.c>
