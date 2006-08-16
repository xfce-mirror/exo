/* $Id$ */
/*-
 * Copyright (c) 2003-2006 Benedikt Meurer <benny@xfce.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <exo-helper/exo-helper-chooser-dialog.h>

#include <exo-support/exo-support.h>



static void exo_helper_chooser_dialog_init      (ExoHelperChooserDialog *chooser_dialog);
static void exo_helper_chooser_dialog_show_help (ExoHelperChooserDialog *dialog);



struct _ExoHelperChooserDialogClass
{
  XfceTitledDialogClass __parent__;
};

struct _ExoHelperChooserDialog
{
  XfceTitledDialog __parent__;
};



GType
exo_helper_chooser_dialog_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ExoHelperChooserDialogClass),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        sizeof (ExoHelperChooserDialog),
        0,
        (GInstanceInitFunc) exo_helper_chooser_dialog_init,
        NULL,
      };

      type = g_type_register_static (XFCE_TYPE_TITLED_DIALOG, I_("ExoHelperChooserDialog"), &info, 0);
    }

  return type;
}



static void
exo_helper_chooser_dialog_init (ExoHelperChooserDialog *chooser_dialog)
{
  PangoAttribute *attribute;
  PangoAttrList  *attr_list_bold;
  AtkRelationSet *relations;
  AtkRelation    *relation;
  AtkObject      *object;
  GtkWidget      *notebook;
  GtkWidget      *chooser;
  GtkWidget      *button;
  GtkWidget      *frame;
  GtkWidget      *label;
  GtkWidget      *vbox;
  GtkWidget      *box;

  /* verify category settings */
  g_assert (EXO_HELPER_N_CATEGORIES == 3);

  gtk_dialog_add_button (GTK_DIALOG (chooser_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
  gtk_dialog_set_has_separator (GTK_DIALOG (chooser_dialog), FALSE);
  gtk_window_set_icon_name (GTK_WINDOW (chooser_dialog), "preferences-desktop-default-applications");
  gtk_window_set_title (GTK_WINDOW (chooser_dialog), _("Preferred Applications"));
  xfce_titled_dialog_set_subtitle (XFCE_TITLED_DIALOG (chooser_dialog), _("Select default applications for various services"));

  /* add the "Help" button */
  button = gtk_button_new_from_stock (GTK_STOCK_HELP);
  g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK (exo_helper_chooser_dialog_show_help), chooser_dialog);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (chooser_dialog)->action_area), button, FALSE, TRUE, 0);
  gtk_button_box_set_child_secondary (GTK_BUTTON_BOX (GTK_DIALOG (chooser_dialog)->action_area), button, TRUE);
  gtk_widget_show (button);

  notebook = gtk_notebook_new ();
  gtk_container_set_border_width (GTK_CONTAINER (notebook), 6);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (chooser_dialog)->vbox), notebook, TRUE, TRUE, 0);
  gtk_widget_show (notebook);

  /* allocate shared bold label attributes */
  attr_list_bold = pango_attr_list_new ();
  attribute = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
  attribute->start_index = 0;
  attribute->end_index = -1;
  pango_attr_list_insert (attr_list_bold, attribute);

  /*
     Internet
   */
  label = gtk_label_new (_("Internet"));
  vbox = g_object_new (GTK_TYPE_VBOX, "border-width", 12, "spacing", 24, NULL);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);
  gtk_widget_show (label);
  gtk_widget_show (vbox);

  /*
     Web Browser
   */
  frame = g_object_new (GTK_TYPE_FRAME, "border-width", 0, "shadow-type", GTK_SHADOW_NONE, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, TRUE, 0);
  gtk_widget_show (frame);

  label = g_object_new (GTK_TYPE_LABEL, "attributes", attr_list_bold, "label", _("Web Browser"), NULL);
  gtk_frame_set_label_widget (GTK_FRAME (frame), label);
  gtk_widget_show (label);

  box = g_object_new (GTK_TYPE_VBOX, "border-width", 12, "spacing", 12, NULL);
  gtk_container_add (GTK_CONTAINER (frame), box);
  gtk_widget_show (box);

  label = gtk_label_new (_("The preferred Web Browser will be used to open\n"
                           "hyperlinks and display help contents."));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  chooser = exo_helper_chooser_new (EXO_HELPER_WEBBROWSER);
  gtk_box_pack_start (GTK_BOX (box), chooser, FALSE, FALSE, 0);
  gtk_widget_show (chooser);

  /* set Atk label relation for the chooser */
  object = gtk_widget_get_accessible (chooser);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  /*
     Mail Reader
   */
  frame = g_object_new (GTK_TYPE_FRAME, "border-width", 0, "shadow-type", GTK_SHADOW_NONE, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, TRUE, 0);
  gtk_widget_show (frame);

  label = g_object_new (GTK_TYPE_LABEL, "attributes", attr_list_bold, "label", _("Mail Reader"), NULL);
  gtk_frame_set_label_widget (GTK_FRAME (frame), label);
  gtk_widget_show (label);

  box = g_object_new (GTK_TYPE_VBOX, "border-width", 12, "spacing", 12, NULL);
  gtk_container_add (GTK_CONTAINER (frame), box);
  gtk_widget_show (box);

  label = gtk_label_new (_("The preferred Mail Reader will be used to compose\n"
                           "emails when you click on email addresses."));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  chooser = exo_helper_chooser_new (EXO_HELPER_MAILREADER);
  gtk_box_pack_start (GTK_BOX (box), chooser, FALSE, FALSE, 0);
  gtk_widget_show (chooser);

  /* set Atk label relation for the chooser */
  object = gtk_widget_get_accessible (chooser);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  /*
     Utilities
   */
  label = gtk_label_new (_("Utilities"));
  vbox = g_object_new (GTK_TYPE_VBOX, "border-width", 12, "spacing", 24, NULL);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);
  gtk_widget_show (label);
  gtk_widget_show (vbox);

  /*
     Terminal Emulator
   */
  frame = g_object_new (GTK_TYPE_FRAME, "border-width", 0, "shadow-type", GTK_SHADOW_NONE, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, TRUE, 0);
  gtk_widget_show (frame);

  label = g_object_new (GTK_TYPE_LABEL, "attributes", attr_list_bold, "label", _("Terminal Emulator"), NULL);
  gtk_frame_set_label_widget (GTK_FRAME (frame), label);
  gtk_widget_show (label);

  box = g_object_new (GTK_TYPE_VBOX, "border-width", 12, "spacing", 12, NULL);
  gtk_container_add (GTK_CONTAINER (frame), box);
  gtk_widget_show (box);

  label = gtk_label_new (_("The preferred Terminal Emulator will be used to\n"
                           "run commands that require a CLI environment."));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  chooser = exo_helper_chooser_new (EXO_HELPER_TERMINALEMULATOR);
  gtk_box_pack_start (GTK_BOX (box), chooser, FALSE, FALSE, 0);
  gtk_widget_show (chooser);

  /* set Atk label relation for the chooser */
  object = gtk_widget_get_accessible (chooser);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  /* cleanup */
  pango_attr_list_unref (attr_list_bold);
}



static void
exo_helper_chooser_dialog_show_help (ExoHelperChooserDialog *chooser_dialog)
{
  GtkWidget *message;
  GError    *error = NULL;

  g_return_if_fail (EXO_IS_HELPER_CHOOSER_DIALOG (chooser_dialog));

  /* try to open the documentation using xfhelp4 */
  if (!gdk_spawn_command_line_on_screen (gtk_widget_get_screen (GTK_WIDGET (chooser_dialog)), "xfhelp4 exo-preferred-applications.html", &error))
    {
      message = gtk_message_dialog_new (GTK_WINDOW (chooser_dialog),
                                        GTK_DIALOG_DESTROY_WITH_PARENT
                                        | GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("Failed to open the documentation browser."));
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s.", error->message);
      gtk_dialog_run (GTK_DIALOG (message));
      gtk_widget_destroy (message);
      g_error_free (error);
    }
}



/**
 * exo_helper_chooser_dialog_new:
 *
 * Allocates a new #ExoHelperChooserDialog.
 *
 * Return value: the newly allocated #ExoHelperChooserDialog.
 **/
GtkWidget*
exo_helper_chooser_dialog_new (void)
{
  return g_object_new (EXO_TYPE_HELPER_CHOOSER_DIALOG, NULL);
}


