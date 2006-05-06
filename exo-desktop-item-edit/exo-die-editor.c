/* $Id$ */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>.
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

#include <exo-desktop-item-edit/exo-die-command-entry.h>
#include <exo-desktop-item-edit/exo-die-editor.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_COMPLETE,
  PROP_MODE,
  PROP_NAME,
  PROP_COMMENT,
  PROP_COMMAND,
  PROP_URL,
  PROP_ICON,
  PROP_SNOTIFY,
  PROP_TERMINAL,
};



static void exo_die_editor_class_init   (ExoDieEditorClass  *klass);
static void exo_die_editor_init         (ExoDieEditor       *editor);
static void exo_die_editor_finalize     (GObject            *object);
static void exo_die_editor_get_property (GObject            *object,
                                         guint               prop_id,
                                         GValue             *value,
                                         GParamSpec         *pspec);
static void exo_die_editor_set_property (GObject            *object,
                                         guint               prop_id,
                                         const GValue       *value,
                                         GParamSpec         *pspec);
static void exo_die_editor_icon_clicked (GtkWidget          *button,
                                         ExoDieEditor       *editor);



struct _ExoDieEditorClass
{
  GtkTableClass __parent__;
};

struct _ExoDieEditor
{
  GtkTable         __parent__;
  GtkWidget       *icon_button;
  GtkTooltips     *tooltips;
  ExoDieEditorMode mode;
  gchar           *name;
  gchar           *comment;
  gchar           *command;
  gchar           *url;
  gchar           *icon;
  guint            snotify : 1;
  guint            terminal : 1;
};



static GObjectClass *exo_die_editor_parent_class;



GType
exo_die_editor_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ExoDieEditorClass),
        NULL,
        NULL,
        (GClassInitFunc) exo_die_editor_class_init,
        NULL,
        NULL,
        sizeof (ExoDieEditor),
        0,
        (GInstanceInitFunc) exo_die_editor_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_TABLE, I_("ExoDieEditor"), &info, 0);
    }

  return type;
}



static void
exo_die_editor_class_init (ExoDieEditorClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  exo_die_editor_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_die_editor_finalize;
  gobject_class->get_property = exo_die_editor_get_property;
  gobject_class->set_property = exo_die_editor_set_property;

  /**
   * ExoDieEditor:complete:
   *
   * %TRUE if the values entered into the #ExoDieEditor are
   * complete for the given mode.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COMPLETE,
                                   g_param_spec_boolean ("complete",
                                                         "complete",
                                                         "complete",
                                                         FALSE,
                                                         EXO_PARAM_READABLE));

  /**
   * ExoDieEditor:mode:
   *
   * The #ExoDieEditorMode for this editor.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_MODE,
                                   g_param_spec_enum ("mode", "mode", "mode",
                                                      EXO_DIE_TYPE_EDITOR_MODE,
                                                      EXO_DIE_EDITOR_MODE_APPLICATION,
                                                      EXO_PARAM_READWRITE));

  /**
   * ExoDieEditor:name:
   *
   * The name of the desktop item edited by this editor.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "name",
                                                        "name",
                                                        NULL,
                                                        EXO_PARAM_READWRITE));

  /**
   * ExoDieEditor:comment:
   *
   * The commment of the desktop item edited by this editor.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COMMENT,
                                   g_param_spec_string ("comment",
                                                        "comment",
                                                        "comment",
                                                        NULL,
                                                        EXO_PARAM_READWRITE));

  /**
   * ExoDieEditor:command:
   *
   * The command of the desktop item edited by this editor, only
   * valid if the mode is %EXO_DIE_EDITOR_MODE_APPLICATION.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COMMAND,
                                   g_param_spec_string ("command",
                                                        "command",
                                                        "command",
                                                        NULL,
                                                        EXO_PARAM_READWRITE));

  /**
   * ExoDieEditor:url:
   *
   * The URL of the desktop item edited by this editor, only valid
   * if the mode is %EXO_DIE_EDITOR_MODE_LINK.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_URL,
                                   g_param_spec_string ("url",
                                                        "url",
                                                        "url",
                                                        NULL,
                                                        EXO_PARAM_READWRITE));

  /**
   * ExoDieEditor:icon:
   *
   * The icon of the desktop item edited by this editor.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ICON,
                                   g_param_spec_string ("icon",
                                                        "icon",
                                                        "icon",
                                                        NULL,
                                                        EXO_PARAM_READWRITE));

  /**
   * ExoDieEditor:snotify:
   *
   * Whether the desktop item edited by this editor should use startup
   * notification, only valid if mode is %EXO_DIE_EDITOR_MODE_APPLICATION.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_SNOTIFY,
                                   g_param_spec_boolean ("snotify",
                                                         "snotify",
                                                         "snotify",
                                                         FALSE,
                                                         EXO_PARAM_READWRITE));

  /**
   * ExoDieEditor:terminal:
   *
   * Whether the desktop item edited by this editor should have its command
   * run in a terminal, only valid if mode is %EXO_DIE_EDITOR_MODE_APPLICATION.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TERMINAL,
                                   g_param_spec_boolean ("terminal",
                                                         "terminal",
                                                         "terminal",
                                                         FALSE,
                                                         EXO_PARAM_READWRITE));
}



static gboolean
exo_die_true_if_application (const GValue *src_value,
                             GValue       *dst_value,
                             gpointer      user_data)
{
  g_value_set_boolean (dst_value, (g_value_get_enum (src_value) == EXO_DIE_EDITOR_MODE_APPLICATION));
  return TRUE;
}



static gboolean
exo_die_true_if_link (const GValue *src_value,
                      GValue       *dst_value,
                      gpointer      user_data)
{
  g_value_set_boolean (dst_value, (g_value_get_enum (src_value) == EXO_DIE_EDITOR_MODE_LINK));
  return TRUE;
}



static void
exo_die_editor_init (ExoDieEditor *editor)
{
  GtkWidget *button;
  GtkWidget *align;
  GtkWidget *entry;
  GtkWidget *label;
  gchar     *text;
  gint       row;

  /* start with sane defaults */
  editor->command = g_strdup ("");
  editor->comment = g_strdup ("");
  editor->icon = g_strdup ("");
  editor->name = g_strdup ("");
  editor->url = g_strdup ("");

  /* allocate the shared tooltips */
  editor->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (editor->tooltips));

  /* configure the table */
  gtk_table_resize (GTK_TABLE (editor), 8, 2);
  gtk_table_set_col_spacings (GTK_TABLE (editor), 12);
  gtk_table_set_row_spacings (GTK_TABLE (editor), 0);

  row = 0;

  /* TRANSLATORS: Label in "Create Launcher"/"Create Link" dialog, make sure to avoid mnemonic conflicts */
  text = g_strdup_printf ("<b>%s</b>", _("_Name:"));
  label = gtk_label_new_with_mnemonic (text);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (editor), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);
  g_free (text);

  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (editor), "name", G_OBJECT (entry), "text");
  gtk_table_attach (GTK_TABLE (editor), entry, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
  gtk_widget_show (entry);

  row += 1;

  /* TRANSLATORS: Label in "Create Launcher"/"Create Link" dialog, make sure to avoid mnemonic conflicts */
  text = g_strdup_printf ("<b>%s</b>", _("C_omment:"));
  label = gtk_label_new_with_mnemonic (text);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (editor), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);
  g_free (text);

  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (editor), "comment", G_OBJECT (entry), "text");
  gtk_table_attach (GTK_TABLE (editor), entry, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
  gtk_widget_show (entry);

  row += 1;

  /* TRANSLATORS: Label in "Create Launcher" dialog, make sure to avoid mnemonic conflicts */
  text = g_strdup_printf ("<b>%s</b>", _("Comm_and:"));
  label = gtk_label_new_with_mnemonic (text);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  exo_binding_new_full (G_OBJECT (editor), "mode", G_OBJECT (label), "visible", exo_die_true_if_application, NULL, NULL);
  gtk_table_attach (GTK_TABLE (editor), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  g_free (text);

  entry = exo_die_command_entry_new ();
  exo_mutual_binding_new (G_OBJECT (editor), "command", G_OBJECT (entry), "text");
  exo_binding_new_full (G_OBJECT (editor), "mode", G_OBJECT (entry), "visible", exo_die_true_if_application, NULL, NULL);
  gtk_table_attach (GTK_TABLE (editor), entry, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);

  row += 1;

  /* TRANSLATORS: Label in "Create Link" dialog, make sure to avoid mnemonic conflicts */
  text = g_strdup_printf ("<b>%s</b>", _("_URL:"));
  label = gtk_label_new_with_mnemonic (text);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  exo_binding_new_full (G_OBJECT (editor), "mode", G_OBJECT (label), "visible", exo_die_true_if_link, NULL, NULL);
  gtk_table_attach (GTK_TABLE (editor), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  g_free (text);

  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (editor), "url", G_OBJECT (entry), "text");
  exo_binding_new_full (G_OBJECT (editor), "mode", G_OBJECT (entry), "visible", exo_die_true_if_link, NULL, NULL);
  gtk_table_attach (GTK_TABLE (editor), entry, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);

  row += 1;

  /* TRANSLATORS: Label in "Create Launcher"/"Create Link" dialog, make sure to avoid mnemonic conflicts */
  text = g_strdup_printf ("<b>%s</b>", _("_Icon:"));
  label = gtk_label_new_with_mnemonic (text);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (editor), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);
  g_free (text);

  align = gtk_alignment_new (0.0f, 0.5f, 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (editor), align, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (align);

  editor->icon_button = gtk_button_new ();
  g_signal_connect (G_OBJECT (editor->icon_button), "clicked", G_CALLBACK (exo_die_editor_icon_clicked), editor);
  gtk_container_add (GTK_CONTAINER (align), editor->icon_button);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), editor->icon_button);
  gtk_widget_show (editor->icon_button);

  /* TRANSLATORS: Label for the icon button in "Create Launcher"/"Create Link" dialog if no icon selected */
  label = gtk_label_new (_("No icon"));
  gtk_container_add (GTK_CONTAINER (editor->icon_button), label);
  gtk_widget_show (label);
  
  row += 1;

  /* add an empty row to get some spacing before the options */
  gtk_table_set_row_spacing (GTK_TABLE (editor), row, 18);

  row += 1;

  text = g_strdup_printf ("<b>%s</b>", _("Options:"));
  label = gtk_label_new_with_mnemonic (text);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  exo_binding_new_full (G_OBJECT (editor), "mode", G_OBJECT (label), "visible", exo_die_true_if_application, NULL, NULL);
  gtk_table_attach (GTK_TABLE (editor), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  g_free (text);

  /* TRANSLATORS: Check button label in "Create Launcher" dialog, make sure to avoid mnemonic conflicts
   *              and sync your translations with the translations in Thunar and xfce4-panel.
   */
  button = gtk_check_button_new_with_mnemonic (_("Use _startup notification"));
  gtk_tooltips_set_tip (editor->tooltips, button, _("Select this option to enable startup notification when the command "
                                                    "is run from the file manager or the menu. Not every application supports "
                                                    "startup notification."), NULL);
  exo_mutual_binding_new (G_OBJECT (editor), "snotify", G_OBJECT (button), "active");
  exo_binding_new_full (G_OBJECT (editor), "mode", G_OBJECT (button), "visible", exo_die_true_if_application, NULL, NULL);
  gtk_table_attach (GTK_TABLE (editor), button, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);

  row += 1;

  /* TRANSLATORS: Check button label in "Create Launcher" dialog, make sure to avoid mnemonic conflicts
   *              and sync your translations with the translations in Thunar and xfce4-panel.
   */
  button = gtk_check_button_new_with_mnemonic (_("Run in _terminal"));
  gtk_tooltips_set_tip (editor->tooltips, button, _("Select this option to run the command in a terminal window."), NULL);
  exo_mutual_binding_new (G_OBJECT (editor), "terminal", G_OBJECT (button), "active");
  exo_binding_new_full (G_OBJECT (editor), "mode", G_OBJECT (button), "visible", exo_die_true_if_application, NULL, NULL);
  gtk_table_attach (GTK_TABLE (editor), button, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
}



static void
exo_die_editor_finalize (GObject *object)
{
  ExoDieEditor *editor = EXO_DIE_EDITOR (object);

  /* cleanup */
  g_object_unref (G_OBJECT (editor->tooltips));
  g_free (editor->command);
  g_free (editor->comment);
  g_free (editor->icon);
  g_free (editor->name);
  g_free (editor->url);

  (*G_OBJECT_CLASS (exo_die_editor_parent_class)->finalize) (object);
}



static void
exo_die_editor_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  ExoDieEditor *editor = EXO_DIE_EDITOR (object);

  switch (prop_id)
    {
    case PROP_COMPLETE:
      g_value_set_boolean (value, exo_die_editor_get_complete (editor));
      break;

    case PROP_MODE:
      g_value_set_enum (value, exo_die_editor_get_mode (editor));
      break;

    case PROP_NAME:
      g_value_set_string (value, exo_die_editor_get_name (editor));
      break;

    case PROP_COMMENT:
      g_value_set_string (value, exo_die_editor_get_comment (editor));
      break;

    case PROP_COMMAND:
      g_value_set_string (value, exo_die_editor_get_command (editor));
      break;

    case PROP_URL:
      g_value_set_string (value, exo_die_editor_get_url (editor));
      break;

    case PROP_ICON:
      g_value_set_string (value, exo_die_editor_get_icon (editor));
      break;

    case PROP_SNOTIFY:
      g_value_set_boolean (value, exo_die_editor_get_snotify (editor));
      break;

    case PROP_TERMINAL:
      g_value_set_boolean (value, exo_die_editor_get_terminal (editor));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_die_editor_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  ExoDieEditor *editor = EXO_DIE_EDITOR (object);

  switch (prop_id)
    {
    case PROP_MODE:
      exo_die_editor_set_mode (editor, g_value_get_enum (value));
      break;

    case PROP_NAME:
      exo_die_editor_set_name (editor, g_value_get_string (value));
      break;

    case PROP_COMMENT:
      exo_die_editor_set_comment (editor, g_value_get_string (value));
      break;

    case PROP_COMMAND:
      exo_die_editor_set_command (editor, g_value_get_string (value));
      break;

    case PROP_URL:
      exo_die_editor_set_url (editor, g_value_get_string (value));
      break;

    case PROP_ICON:
      exo_die_editor_set_icon (editor, g_value_get_string (value));
      break;

    case PROP_SNOTIFY:
      exo_die_editor_set_snotify (editor, g_value_get_boolean (value));
      break;

    case PROP_TERMINAL:
      exo_die_editor_set_terminal (editor, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
update_preview (GtkFileChooser *chooser,
                GtkWidget      *image)
{
  GdkPixbuf *pixbuf;
  GdkPixbuf *scaled;
  gchar     *filename;
  gint       height;
  gint       width;

  filename = gtk_file_chooser_get_filename (chooser);
  pixbuf = (filename != NULL) ? gdk_pixbuf_new_from_file (filename, NULL) : NULL;
  if (G_LIKELY (pixbuf != NULL))
    {
      width = gdk_pixbuf_get_width (pixbuf);
      height = gdk_pixbuf_get_height (pixbuf);
      if (G_UNLIKELY (width > 128 || height > 128))
        {
          scaled = exo_gdk_pixbuf_scale_ratio (pixbuf, 128);
          g_object_unref (G_OBJECT (pixbuf));
          pixbuf = scaled;
        }

      gtk_file_chooser_set_preview_widget_active (chooser, TRUE);
      gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbuf);
      g_object_unref (G_OBJECT (pixbuf));
    }
  else
    {
      gtk_file_chooser_set_preview_widget_active (chooser, FALSE);
    }

  g_free (filename);
}



static void
exo_die_editor_icon_clicked (GtkWidget    *button,
                             ExoDieEditor *editor)
{
  GtkFileFilter *filter;
  GtkIconTheme  *icon_theme;
  GtkIconInfo   *icon_info;
  GtkWidget     *toplevel;
  GtkWidget     *chooser;
  GtkWidget     *image;
  gchar        **extensions;
  gchar        **mime_types;
  gchar         *filename = NULL;
  gchar         *pattern;
  GSList        *formats;
  GSList        *lp;
  gint           n;

  g_return_if_fail (GTK_IS_BUTTON (button));
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));

  /* determine the toplevel widget */
  toplevel = gtk_widget_get_toplevel (button);
  if (toplevel == NULL || !GTK_WIDGET_TOPLEVEL (toplevel))
    return;

  chooser = gtk_file_chooser_dialog_new (_("Select an Icon"),
                                         GTK_WINDOW (toplevel),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);
  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (chooser), TRUE);

  /* add file chooser filters */
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("All Files"));
  gtk_file_filter_add_pattern (filter, "*");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Image Files"));
  formats = gdk_pixbuf_get_formats ();
  for (lp = formats; lp != NULL; lp = lp->next)
    {
      extensions = gdk_pixbuf_format_get_extensions (lp->data);
      for (n = 0; extensions[n] != NULL; ++n)
        {
          pattern = g_strconcat ("*.", extensions[n], NULL);
          gtk_file_filter_add_pattern (filter, pattern);
          g_free (pattern);
        }
      g_strfreev (extensions);

      mime_types = gdk_pixbuf_format_get_mime_types (lp->data);
      for (n = 0; mime_types[n] != NULL; ++n)
        gtk_file_filter_add_mime_type (filter, mime_types[n]);
      g_strfreev (mime_types);
    }
  g_slist_free (formats);
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (chooser), filter);

  /* add preview widget */
  image = g_object_new (GTK_TYPE_IMAGE, "height-request", 128 + 2 * 12, "width-request", 128 + 2 * 12, NULL);
  g_signal_connect (G_OBJECT (chooser), "update-preview", G_CALLBACK (update_preview), image);
  gtk_file_chooser_set_preview_widget_active (GTK_FILE_CHOOSER (chooser), FALSE);
  gtk_file_chooser_set_preview_widget (GTK_FILE_CHOOSER (chooser), image);
  gtk_widget_show (image);

  /* use the datadir/pixmaps as default folder */
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooser), DATADIR "/pixmaps");

  /* check if we have an icon set */
  if (G_LIKELY (editor->icon != NULL && *editor->icon != '\0'))
    {
      /* check the icon type */
      if (G_LIKELY (g_path_is_absolute (editor->icon)))
        {
          /* just use the icon file path */
          filename = g_strdup (editor->icon);
        }
      else
        {
          /* determine the appropriate icon theme */
          icon_theme = gtk_icon_theme_get_for_screen (gtk_widget_get_screen (chooser));

          /* try to lookup the icon on the theme */
          icon_info = gtk_icon_theme_lookup_icon (icon_theme, editor->icon, 48, 0);
          if (G_LIKELY (icon_info != NULL))
            {
              /* use the filename from the icon info */
              filename = g_strdup (gtk_icon_info_get_filename (icon_info));

              /* release the icon info */
              gtk_icon_info_free (icon_info);
            }
        }

      /* setup the currently selected icon file */
      if (G_LIKELY (filename != NULL && g_path_is_absolute (filename)))
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (chooser), filename);

      /* release the filename */
      g_free (filename);
    }

  /* run the chooser dialog */
  if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT)
    {
      /* remember the selected file for the icon */
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
      exo_die_editor_set_icon (editor, filename);
      g_free (filename);
    }

  /* destroy the chooser */
  gtk_widget_destroy (chooser);
}



/**
 * exo_die_editor_new:
 *
 * Allocates a new #ExoDieEditor instance.
 *
 * Return value: the newly allocated #ExoDieEditor.
 **/
GtkWidget*
exo_die_editor_new (void)
{
  return g_object_new (EXO_DIE_TYPE_EDITOR, NULL);
}



/**
 * exo_die_dialog_get_complete:
 * @editor : an #ExoDieEditor.
 *
 * Returns %TRUE if the values entered into the
 * @editor are complete.
 *
 * Return value: if @editor<!---->s values are
 *               complete.
 **/
gboolean
exo_die_editor_get_complete (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), FALSE);

  /* the exact meaning of complete depends on the mode */
  switch (editor->mode)
    {
    case EXO_DIE_EDITOR_MODE_APPLICATION:
      return (*editor->name != '\0' && *editor->command != '\0');

    case EXO_DIE_EDITOR_MODE_LINK:
      return (*editor->name != '\0' && *editor->url != '\0');

    default:
      g_assert_not_reached ();
      return FALSE;
    }
}



/**
 * exo_die_editor_get_mode:
 * @editor : an #ExoDieEditor.
 *
 * Returns the #ExoDieEditorMode for @editor.
 *
 * Return value: the #ExoDieEditorMode for @editor.
 **/
ExoDieEditorMode
exo_die_editor_get_mode (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), EXO_DIE_EDITOR_MODE_APPLICATION);
  return editor->mode;
}



/**
 * exo_die_editor_set_mode:
 * @editor : an #ExoDieEditor.
 * @mode   : an #ExoDieEditorMode.
 *
 * Sets the mode for @editor to @mode.
 **/
void
exo_die_editor_set_mode (ExoDieEditor    *editor,
                         ExoDieEditorMode mode)
{
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));

  /* check if we have a new mode here */
  if (G_LIKELY (editor->mode != mode))
    {
      /* apply the new mode */
      editor->mode = mode;

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "complete");
      g_object_notify (G_OBJECT (editor), "mode");
    }
}



/**
 * exo_die_editor_get_name:
 * @editor : an #ExoDieEditor.
 *
 * Returns the name for the @editor.
 *
 * Return value: the name for the @editor.
 **/
const gchar*
exo_die_editor_get_name (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), NULL);
  return editor->name;
}



/**
 * exo_die_editor_set_name:
 * @editor : an #ExoDieEditor.
 * @name   : the new name for @editor.
 *
 * Sets the name for @editor to @name.
 **/
void
exo_die_editor_set_name (ExoDieEditor *editor,
                         const gchar  *name)
{
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));
  g_return_if_fail (g_utf8_validate (name, -1, NULL));

  /* check if we have a new name */
  if (!exo_str_is_equal (editor->name, name))
    {
      /* apply the new name */
      g_free (editor->name);
      editor->name = g_strdup (name);

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "complete");
      g_object_notify (G_OBJECT (editor), "name");
    }
}



/**
 * exo_die_editor_get_comment:
 * @editor : an #ExoDieEditor.
 *
 * Returns the comment for @editor.
 *
 * Return value: the comment for @editor.
 **/
const gchar*
exo_die_editor_get_comment (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), NULL);
  return editor->comment;
}



/**
 * exo_die_editor_set_comment:
 * @editor  : an #ExoDieEditor.
 * @comment : the new comment for @editor.
 *
 * Sets the comment for @editor to @comment.
 **/
void
exo_die_editor_set_comment (ExoDieEditor *editor,
                            const gchar  *comment)
{
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));
  g_return_if_fail (g_utf8_validate (comment, -1, NULL));

  /* check if we have a new comment here */
  if (!exo_str_is_equal (editor->comment, comment))
    {
      /* apply the new comment */
      g_free (editor->comment);
      editor->comment = g_strdup (comment);

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "comment");
    }
}



/**
 * exo_die_editor_get_command:
 * @editor : an #ExoDieEditor.
 *
 * Returns the command for the @editor, which is only valid
 * if the mode is %EXO_DIE_EDITOR_MODE_APPLICATION.
 *
 * Return value: the command for the @editor.
 **/
const gchar*
exo_die_editor_get_command (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), NULL);
  return editor->command;
}



/**
 * exo_die_editor_set_command:
 * @editor  : an #ExoDieEditor.
 * @command : the new command for @editor.
 *
 * Sets the command for @editor to @command.
 **/
void
exo_die_editor_set_command (ExoDieEditor *editor,
                            const gchar  *command)
{
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));
  g_return_if_fail (g_utf8_validate (command, -1, NULL));

  /* check if we have a new command here */
  if (!exo_str_is_equal (editor->command, command))
    {
      /* apply the new command */
      g_free (editor->command);
      editor->command = g_strdup (command);

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "complete");
      g_object_notify (G_OBJECT (editor), "command");
    }
}



/**
 * exo_die_editor_get_url:
 * @editor : an #ExoDieEditor.
 *
 * Returns the URL for @editor, which is only valid
 * if the mode is %EXO_DIE_EDITOR_MODE_LINK.
 *
 * Return value: the URL for @editor.
 **/
const gchar*
exo_die_editor_get_url (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), NULL);
  return editor->url;
}



/**
 * exo_die_editor_set_url:
 * @editor : an #ExoDieEditor.
 * @url    : the new URL for @editor.
 *
 * Sets the URL for @editor to @url.
 **/
void
exo_die_editor_set_url (ExoDieEditor *editor,
                        const gchar  *url)
{
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));
  g_return_if_fail (g_utf8_validate (url, -1, NULL));

  /* check if we have a new URL here */
  if (!exo_str_is_equal (editor->url, url))
    {
      /* apply the new URL */
      g_free (editor->url);
      editor->url = g_strdup (url);

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "complete");
      g_object_notify (G_OBJECT (editor), "url");
    }
}



/**
 * exo_die_editor_get_icon:
 * @editor : an #ExoDieEditor.
 *
 * Returns the icon for the @editor.
 *
 * Return value: the icon for the @editor.
 **/
const gchar*
exo_die_editor_get_icon (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), NULL);
  return editor->icon;
}



/**
 * exo_die_editor_set_icon:
 * @editor : an #ExoDieEditor.
 * @icon   : the new icon for the @editor.
 *
 * Sets the icon for the @editor to @icon.
 **/
void
exo_die_editor_set_icon (ExoDieEditor *editor,
                         const gchar  *icon)
{
  GtkIconTheme *icon_theme;
  GdkPixbuf    *pixbuf_scaled;
  GdkPixbuf    *pixbuf = NULL;
  GtkWidget    *image;
  GtkWidget    *label;
  gint          pixbuf_width;
  gint          pixbuf_height;

  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));
  g_return_if_fail (g_utf8_validate (icon, -1, NULL));

  /* check if we have a new icon here */
  if (!exo_str_is_equal (editor->icon, icon))
    {
      /* apply the new icon */
      g_free (editor->icon);
      editor->icon = g_strdup (icon);

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "icon");

      /* drop the previous icon button child */
      if (GTK_BIN (editor->icon_button)->child != NULL)
        gtk_widget_destroy (GTK_BIN (editor->icon_button)->child);

      /* check the icon depending on the type */
      if (icon != NULL && g_path_is_absolute (icon))
        {
          /* try to load the icon from the file */
          pixbuf = gdk_pixbuf_new_from_file (icon, NULL);
        }
      else if (icon != NULL && *icon != '\0')
        {
          /* determine the appropriate icon theme */
          icon_theme = gtk_icon_theme_get_for_screen (gtk_widget_get_screen (GTK_WIDGET (editor)));

          /* try to load the named icon */
          pixbuf = gtk_icon_theme_load_icon (icon_theme, icon, 48, 0, NULL);
        }

      /* setup the icon button */
      if (G_LIKELY (pixbuf != NULL))
        {
          /* scale down the icon if required */
          pixbuf_width = gdk_pixbuf_get_width (pixbuf);
          pixbuf_height = gdk_pixbuf_get_height (pixbuf);
          if (G_UNLIKELY (pixbuf_width > 48 || pixbuf_height > 48))
            {
              pixbuf_scaled = exo_gdk_pixbuf_scale_ratio (pixbuf, 48);
              g_object_unref (G_OBJECT (pixbuf));
              pixbuf = pixbuf_scaled;
            }

          /* setup an image for the icon */
          image = gtk_image_new_from_pixbuf (pixbuf);
          gtk_container_add (GTK_CONTAINER (editor->icon_button), image);
          gtk_widget_show (image);

          /* release the pixbuf */
          g_object_unref (G_OBJECT (pixbuf));
        }
      else
        {
          /* setup a label to tell that no icon was selected */
          label = gtk_label_new (_("No icon"));
          gtk_container_add (GTK_CONTAINER (editor->icon_button), label);
          gtk_widget_show (label);
        }
    }
}



/**
 * exo_die_editor_get_snotify:
 * @editor : an #ExoDieEditor.
 *
 * Returns %TRUE if @editor has enabled startup notification, which
 * is only valid if mode is %EXO_DIE_EDITOR_MODE_APPLICATION.
 *
 * Return value: %TRUE if startup notification is enabled.
 **/
gboolean
exo_die_editor_get_snotify (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), FALSE);
  return editor->snotify;
}



/**
 * exo_die_editor_set_snotify:
 * @editor  : an #ExoDieEditor.
 * @snotify : %TRUE to enable startup notification.
 *
 * Set startup notification state for @editor to @snotify.
 **/
void
exo_die_editor_set_snotify (ExoDieEditor *editor,
                            gboolean      snotify)
{
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));

  /* normalize the value */
  snotify = !!snotify;

  /* check if we have a new value */
  if (editor->snotify != snotify)
    {
      /* apply the new value */
      editor->snotify = snotify;

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "snotify");
    }
}



/**
 * exo_die_editor_get_terminal:
 * @editor : an #ExoDieEditor.
 *
 * Returns %TRUE if the command should be run in a terminal, only valid
 * if mode for @editor is %EXO_DIE_EDITOR_MODE_APPLICATION.
 *
 * Return value: %TRUE if command should be run in terminal.
 **/
gboolean
exo_die_editor_get_terminal (ExoDieEditor *editor)
{
  g_return_val_if_fail (EXO_DIE_IS_EDITOR (editor), FALSE);
  return editor->terminal;
}



/**
 * exo_die_editor_set_terminal:
 * @editor   : an #ExoDieEditor.
 * @terminal : %TRUE to run command in terminal.
 *
 * Sets the run in terminal setting of @editor to @terminal.
 **/
void
exo_die_editor_set_terminal (ExoDieEditor *editor,
                             gboolean      terminal)
{
  g_return_if_fail (EXO_DIE_IS_EDITOR (editor));

  /* normalize the value */
  terminal = !!terminal;

  /* check if we have a new value */
  if (editor->terminal != terminal)
    {
      /* apply the new value */
      editor->terminal = terminal;

      /* notify listeners */
      g_object_notify (G_OBJECT (editor), "terminal");
    }
}





