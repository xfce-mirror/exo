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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libxfce4util/libxfce4util.h>

#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-toolbars-editor.h>
#include <exo/exo-toolbars-editor-dialog.h>
#include <exo/exo-alias.h>

/**
 * SECTION: exo-toolbars-editor-dialog
 * @title: ExoToolbarsEditorDialog
 * @short_description: Dialog to edit toolbars
 * @include: exo/exo.h
 * @see_also: #ExoToolbarsEditor
 *
 * Provides an easy-to-use wrapper for the #ExoToolbarsEditor widget.
 **/



#define EXO_TOOLBARS_EDITOR_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
    EXO_TYPE_TOOLBARS_EDITOR_DIALOG, ExoToolbarsEditorDialogPrivate))



static void exo_toolbars_editor_dialog_add_toolbar  (ExoToolbarsEditorDialog      *dialog);



struct _ExoToolbarsEditorDialogPrivate
{
  GtkWidget *editor;
};



G_DEFINE_TYPE (ExoToolbarsEditorDialog, exo_toolbars_editor_dialog, GTK_TYPE_DIALOG)



static void
exo_toolbars_editor_dialog_class_init (ExoToolbarsEditorDialogClass *klass)
{
  g_type_class_add_private (klass, sizeof (ExoToolbarsEditorDialogPrivate));
}



static void
exo_toolbars_editor_dialog_init (ExoToolbarsEditorDialog *dialog)
{
  GtkWidget *button;
  GtkWidget *align;
  GtkWidget *hbox;
  GtkWidget *image;
  GtkWidget *label;

  dialog->priv = EXO_TOOLBARS_EDITOR_DIALOG_GET_PRIVATE (dialog);

  gtk_window_set_default_size (GTK_WINDOW (dialog), -1, 300);

  dialog->priv->editor = g_object_new (EXO_TYPE_TOOLBARS_EDITOR, NULL);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), dialog->priv->editor, TRUE, TRUE, 0);
  gtk_widget_show (dialog->priv->editor);

  button = gtk_button_new ();
  g_signal_connect_swapped (G_OBJECT (button), "clicked",
                            G_CALLBACK (exo_toolbars_editor_dialog_add_toolbar), dialog);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_action_area (GTK_DIALOG (dialog))), button, FALSE, TRUE, 0);
  gtk_widget_show (button);

  align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
  gtk_container_add (GTK_CONTAINER (button), align);
  gtk_widget_show (align);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER (align), hbox);
  gtk_widget_show (hbox);

  image = gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_widget_show (image);

  label = gtk_label_new_with_mnemonic (_("_Add a new toolbar"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect_swapped (G_OBJECT (button), "clicked",
                            G_CALLBACK (gtk_widget_destroy), dialog);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_action_area (GTK_DIALOG (dialog))), button, FALSE, TRUE, 0);
  gtk_widget_show (button);
}



static void
exo_toolbars_editor_dialog_add_toolbar (ExoToolbarsEditorDialog *dialog)
{
  ExoToolbarsModel *model;
  gchar            *name;

  model = exo_toolbars_editor_get_model (EXO_TOOLBARS_EDITOR (dialog->priv->editor));
  if (G_LIKELY (model != NULL))
    {
      name = g_strdup_printf ("exo-toolbar-%ld-%d", (glong) getpid (), (gint) time (NULL));
      exo_toolbars_model_add_toolbar (model, -1, name);
      g_free (name);
    }
}



/**
 * exo_toolbars_editor_dialog_new_with_model:
 * @ui_manager  : A #GtkUIManager.
 * @model       : An #ExoToolbarsModel.
 *
 * Creates a new #ExoToolbarsEditorDialog that is associated with
 * @ui_manager and @model.
 *
 * Returns: A new #ExoToolbarsEditorDialog.
 **/
GtkWidget*
exo_toolbars_editor_dialog_new_with_model (GtkUIManager     *ui_manager,
                                           ExoToolbarsModel *model)
{
  ExoToolbarsEditorDialog *dialog;

  g_return_val_if_fail (GTK_IS_UI_MANAGER (ui_manager), NULL);
  g_return_val_if_fail (EXO_IS_TOOLBARS_MODEL (model), NULL);

  dialog = g_object_new (EXO_TYPE_TOOLBARS_EDITOR_DIALOG, NULL);
  g_object_set (G_OBJECT (dialog->priv->editor),
                "ui-manager", ui_manager,
                "model", model,
                NULL);

  return GTK_WIDGET (dialog);
}



#define __EXO_TOOLBARS_EDITOR_DIALOG_C__
#include <exo/exo-aliasdef.c>
