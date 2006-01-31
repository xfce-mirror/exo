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

#include <exo-helper/exo-helper-chooser.h>
#include <exo-helper/exo-helper-enum-types.h>
#include <exo-helper/exo-helper-utils.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_CATEGORY,
  PROP_IS_VALID,
};



static void exo_helper_chooser_class_init   (ExoHelperChooserClass  *klass);
static void exo_helper_chooser_init         (ExoHelperChooser       *chooser);
static void exo_helper_chooser_finalize     (GObject                *object);
static void exo_helper_chooser_get_property (GObject                *object,
                                             guint                   prop_id,
                                             GValue                 *value,
                                             GParamSpec             *pspec);
static void exo_helper_chooser_set_property (GObject                *object,
                                             guint                   prop_id,
                                             const GValue           *value,
                                             GParamSpec             *pspec);
static void exo_helper_chooser_update       (ExoHelperChooser       *chooser);
static void exo_helper_chooser_pressed      (ExoHelperChooser       *chooser,
                                             GtkWidget              *button);



struct _ExoHelperChooserClass
{
  GtkAlignmentClass __parent__;
};

struct _ExoHelperChooser
{
  GtkAlignment __parent__;

  GtkWidget         *image;
  GtkWidget         *label;
  GtkTooltips       *tooltips;

  ExoHelperDatabase *database;
  ExoHelperCategory  category;

  gboolean           is_valid;
};



static GObjectClass *exo_helper_chooser_parent_class;



GType
exo_helper_chooser_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ExoHelperChooserClass),
        NULL,
        NULL,
        (GClassInitFunc) exo_helper_chooser_class_init,
        NULL,
        NULL,
        sizeof (ExoHelperChooser),
        0,
        (GInstanceInitFunc) exo_helper_chooser_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_ALIGNMENT, I_("ExoHelperChooser"), &info, 0);
    }

  return type;
}



static void
exo_helper_chooser_class_init (ExoHelperChooserClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  exo_helper_chooser_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_helper_chooser_finalize;
  gobject_class->get_property = exo_helper_chooser_get_property;
  gobject_class->set_property = exo_helper_chooser_set_property;

  /**
   * ExoHelperChooser:category:
   *
   * The #ExoHelperCategory which should be configured by this
   * #ExoHelperChooser. See exo_helper_chooser_get_category() and
   * exo_helper_chooser_set_category() for details.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_CATEGORY,
                                   g_param_spec_enum ("category",
                                                      "Helper category",
                                                      "Helper category",
                                                      EXO_TYPE_HELPER_CATEGORY,
                                                      EXO_HELPER_WEBBROWSER,
                                                      EXO_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  /**
   * ExoHelperChooser:is-valid:
   *
   * %TRUE if a valid #ExoHelper is selected by
   * this #ExoHelperChooser, else %FALSE.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_IS_VALID,
                                   g_param_spec_boolean ("is-valid",
                                                         "Is valid",
                                                         "Is valid",
                                                         FALSE,
                                                         EXO_PARAM_READABLE));
}



static void
exo_helper_chooser_init (ExoHelperChooser *chooser)
{
  AtkRelationSet *relations;
  AtkRelation    *relation;
  AtkObject      *object;
  GtkWidget      *separator;
  GtkWidget      *button;
  GtkWidget      *arrow;
  GtkWidget      *hbox;

  chooser->database = exo_helper_database_get ();

  chooser->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (chooser->tooltips));

  gtk_widget_push_composite_child ();

  button = gtk_button_new ();
  g_signal_connect_swapped (G_OBJECT (button), "pressed", G_CALLBACK (exo_helper_chooser_pressed), chooser);
  gtk_tooltips_set_tip (chooser->tooltips, button, _("Press left mouse button to change the selected application."), NULL);
  gtk_container_add (GTK_CONTAINER (chooser), button);
  gtk_widget_show (button);

  /* set Atk properties for the button */
  object = gtk_widget_get_accessible (button);
  atk_object_set_name (object, _("Application Chooser Button"));
  atk_object_set_description (object, _("Press left mouse button to change the selected application."));

  hbox = gtk_hbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER (button), hbox);
  gtk_widget_show (hbox);

  chooser->image = gtk_image_new ();
  gtk_box_pack_start (GTK_BOX (hbox), chooser->image, FALSE, FALSE, 0);
  gtk_widget_show (chooser->image);

  chooser->label = g_object_new (GTK_TYPE_LABEL, "xalign", 0.0f, "yalign", 0.0f, NULL);
  gtk_box_pack_start (GTK_BOX (hbox), chooser->label, TRUE, TRUE, 0);
  gtk_widget_show (chooser->label);

  /* set Atk label relation for the button */
  object = gtk_widget_get_accessible (button);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (chooser->label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  separator = g_object_new (GTK_TYPE_VSEPARATOR, "height-request", 16, NULL);
  gtk_box_pack_start (GTK_BOX (hbox), separator, FALSE, FALSE, 0);
  gtk_widget_show (separator);

  arrow = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_NONE);
  gtk_box_pack_start (GTK_BOX (hbox), arrow, FALSE, FALSE, 0);
  gtk_widget_show (arrow);

  gtk_widget_pop_composite_child ();
}



static void
exo_helper_chooser_finalize (GObject *object)
{
  ExoHelperChooser *chooser = EXO_HELPER_CHOOSER (object);

  g_object_unref (G_OBJECT (chooser->database));
  g_object_unref (G_OBJECT (chooser->tooltips));

  (*G_OBJECT_CLASS (exo_helper_chooser_parent_class)->finalize) (object);
}



static void
exo_helper_chooser_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  ExoHelperChooser *chooser = EXO_HELPER_CHOOSER (object);

  switch (prop_id)
    {
    case PROP_CATEGORY:
      g_value_set_enum (value, exo_helper_chooser_get_category (chooser));
      break;

    case PROP_IS_VALID:
      g_value_set_boolean (value, exo_helper_chooser_get_is_valid (chooser));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_helper_chooser_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  ExoHelperChooser *chooser = EXO_HELPER_CHOOSER (object);

  switch (prop_id)
    {
    case PROP_CATEGORY:
      exo_helper_chooser_set_category (chooser, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_helper_chooser_update (ExoHelperChooser *chooser)
{
  GtkIconTheme *icon_theme;
  const gchar  *icon_name;
  ExoHelper    *helper;
  GdkPixbuf    *icon = NULL;
  gint          icon_size;

  g_return_if_fail (EXO_IS_HELPER_CHOOSER (chooser));

  /* determine the default helper for the category */
  helper = exo_helper_database_get_default (chooser->database, chooser->category);
  if (G_LIKELY (helper != NULL))
    {
      /* try to load the icon for the helper */
      icon_name = exo_helper_get_icon (helper);
      if (G_LIKELY (icon_name != NULL))
        {
          gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &icon_size, &icon_size);
          if (g_path_is_absolute (icon_name))
            {
              icon = gdk_pixbuf_new_from_file_at_scale (icon_name, icon_size, icon_size, TRUE, NULL);
            }
          else
            {
              icon_theme = gtk_icon_theme_get_default ();
              icon = gtk_icon_theme_load_icon (icon_theme, icon_name, icon_size, 0, NULL);
            }
        }
      gtk_image_set_from_pixbuf (GTK_IMAGE (chooser->image), icon);
      if (G_LIKELY (icon != NULL))
        g_object_unref (G_OBJECT (icon));

      gtk_label_set_text (GTK_LABEL (chooser->label), exo_helper_get_name (helper));
      g_object_unref (G_OBJECT (helper));
    }
  else
    {
      gtk_image_set_from_pixbuf (GTK_IMAGE (chooser->image), NULL);
      gtk_label_set_text (GTK_LABEL (chooser->label), _("No application selected"));
    }

  /* update the "is-valid" property */
  chooser->is_valid = (helper != NULL);
  g_object_notify (G_OBJECT (chooser), "is-valid");
}



static void
menu_activate (GtkWidget        *item,
               ExoHelperChooser *chooser)
{
  static const gchar *CATEGORY_ERRORS[] =
  {
    N_("Failed to set default Web Browser"),
    N_("Failed to set default Mail Reader"),
    N_("Failed to set default Terminal Emulator"),
  };

  ExoHelper *helper;
  GtkWidget *message;
  GError    *error = NULL;

  /* verify helper category values */
  g_assert (EXO_HELPER_N_CATEGORIES == G_N_ELEMENTS (CATEGORY_ERRORS));

  g_return_if_fail (GTK_IS_WIDGET (item));
  g_return_if_fail (EXO_IS_HELPER_CHOOSER (chooser));

  /* determine the helper for the item */
  helper = g_object_get_data (G_OBJECT (item), I_("exo-helper"));
  if (G_LIKELY (helper != NULL))
    {
      if (!exo_helper_database_set_default (chooser->database, chooser->category, helper, &error))
        {
          message = gtk_message_dialog_new (GTK_WINDOW (chooser),
                                            GTK_DIALOG_DESTROY_WITH_PARENT
                                            | GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_ERROR,
                                            GTK_BUTTONS_CLOSE,
                                            "%s.", _(CATEGORY_ERRORS[chooser->category]));
          gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s.", error->message);
          gtk_dialog_run (GTK_DIALOG (message));
          gtk_widget_destroy (message);
          g_error_free (error);
        }
      else
        {
          /* update the chooser state */
          exo_helper_chooser_update (chooser);
        }
    }
}



static void
entry_changed (GtkEditable *editable,
               GtkDialog   *dialog)
{
  gchar *text;

  text = gtk_editable_get_chars (editable, 0, -1);
  gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, *text != '\0');
  g_free (text);
}



static void
browse_clicked (GtkWidget *button,
                GtkWidget *entry)
{
  GtkWidget *toplevel;
  GtkWidget *dialog;
  gchar    **argv;
  gchar     *filename;
  gchar     *text;

  toplevel = gtk_widget_get_toplevel (entry);
  dialog = gtk_file_chooser_dialog_new (_("Select application"),
                                        GTK_WINDOW (toplevel),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                        NULL);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), BINDIR);
  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (dialog), TRUE);

  filename = gtk_editable_get_chars (GTK_EDITABLE (entry), 0, -1);
  if (G_LIKELY (filename != NULL && g_shell_parse_argv (filename, NULL, &argv, NULL)))
    {
      if (G_LIKELY (*argv != NULL && g_path_is_absolute (*argv)))
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), *argv);
      g_strfreev (argv);
    }
  g_free (filename);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      text = g_strconcat (filename, " \"%s\"", NULL);
      gtk_entry_set_text (GTK_ENTRY (entry), text);
      g_free (filename);
      g_free (text);
    }

  gtk_widget_destroy (dialog);
}



static void
menu_activate_other (GtkWidget        *item,
                     ExoHelperChooser *chooser)
{
  static const gchar *BROWSE_TITLES[] =
  {
    N_("Choose a custom Web Browser"),
    N_("Choose a custom Mail Reader"),
    N_("Choose a custom Terminal Emulator"),
  };

  static const gchar *BROWSE_MESSAGES[] =
  {
    N_("Specify the application you want to use\nas default Web Browser for Xfce:"),
    N_("Specify the application you want to use\nas default Mail Reader for Xfce:"),
    N_("Specify the application you want to use\nas default Terminal Emulator for Xfce:"),
  };

  const gchar *command;
  ExoHelper   *helper;
  GtkWidget   *toplevel;
  GtkWidget   *dialog;
  GtkWidget   *hbox;
  GtkWidget   *image;
  GtkWidget   *vbox;
  GtkWidget   *label;
  GtkWidget   *entry;
  GtkWidget   *button;

  /* sanity check the category values */
  g_assert (EXO_HELPER_N_CATEGORIES == 3);

  toplevel = gtk_widget_get_toplevel (GTK_WIDGET (chooser));

  dialog = gtk_dialog_new_with_buttons (dgettext (GETTEXT_PACKAGE, BROWSE_TITLES[chooser->category]),
                                        GTK_WINDOW (toplevel),
                                        GTK_DIALOG_DESTROY_WITH_PARENT
                                        | GTK_DIALOG_NO_SEPARATOR
                                        | GTK_DIALOG_MODAL,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK, GTK_RESPONSE_OK,
                                        NULL);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
  gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), 12);
  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

  hbox = g_object_new (GTK_TYPE_HBOX, "border-width", 5, "spacing", 12, NULL);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, TRUE, TRUE, 0);
  gtk_widget_show (hbox);

  image = gtk_image_new_from_icon_name ("preferences-desktop-default-applications", GTK_ICON_SIZE_DIALOG);
  gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_widget_show (image);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);

  label = g_object_new (GTK_TYPE_LABEL, 
                        "label", _(BROWSE_MESSAGES[chooser->category]),
                        "xalign", 0.0,
                        "yalign", 0.0,
                        NULL);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
  gtk_widget_show (hbox);

  entry = g_object_new (GTK_TYPE_ENTRY, "activates-default", TRUE, NULL);
  g_signal_connect (G_OBJECT (entry), "changed", G_CALLBACK (entry_changed), dialog);
  gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 0);
  gtk_widget_show (entry);

  button = gtk_button_new ();
  gtk_tooltips_set_tip (chooser->tooltips, button, _("Browse the file system to choose a custom command."), NULL);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (browse_clicked), entry);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
  gtk_container_add (GTK_CONTAINER (button), image);
  gtk_widget_show (image);

  /* set the current custom command (if any) */
  helper = exo_helper_database_get_custom (chooser->database, chooser->category);
  if (G_LIKELY (helper != NULL))
    {
      command = exo_helper_get_command (helper);
      if (G_LIKELY (command != NULL))
        gtk_entry_set_text (GTK_ENTRY (entry), command);
    }

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
    {
      /* change the custom command in the database */
      command = gtk_entry_get_text (GTK_ENTRY (entry));
      exo_helper_database_set_custom (chooser->database, chooser->category, command);

      /* reload the custom helper */
      helper = exo_helper_database_get_custom (chooser->database, chooser->category);
      if (G_LIKELY (helper != NULL))
        {
          /* hide the dialog */
          gtk_widget_hide (dialog);

          /* use menu_activate() to set the custom application as default */
          g_object_set_data_full (G_OBJECT (item), I_("exo-helper"), helper, g_object_unref);
          menu_activate (item, chooser);
        }
    }

  gtk_widget_destroy (dialog);
}



static void
menu_position (GtkMenu  *menu,
               gint     *x,
               gint     *y,
               gboolean *push_in,
               gpointer  chooser)
{
  GtkRequisition chooser_request;
  GtkRequisition menu_request;
  GdkRectangle   geometry;
  GdkScreen     *screen;
  GtkWidget     *toplevel = gtk_widget_get_toplevel (chooser);
  gint           monitor;
  gint           x0;
  gint           y0;

  gtk_widget_translate_coordinates (GTK_WIDGET (chooser), toplevel, 0, 0, &x0, &y0);

  gtk_widget_size_request (GTK_WIDGET (chooser), &chooser_request);
  gtk_widget_size_request (GTK_WIDGET (menu), &menu_request);

  gdk_window_get_position (GTK_WIDGET (chooser)->window, x, y);

  *y += y0;
  *x += x0;

  /* verify the the menu is on-screen */
  screen = gtk_widget_get_screen (GTK_WIDGET (chooser));
  if (G_LIKELY (screen != NULL))
    {
      monitor = gdk_screen_get_monitor_at_point (screen, *x, *y);
      gdk_screen_get_monitor_geometry (screen, monitor, &geometry);
      if (*y + menu_request.height > geometry.y + geometry.height)
        *y -= menu_request.height - chooser_request.height;
    }

  *push_in = TRUE;
}



static void
exo_helper_chooser_pressed (ExoHelperChooser *chooser,
                            GtkWidget        *button)
{
  AtkRelationSet *relations;
  AtkRelation    *relation;
  AtkObject      *object;
  GtkIconTheme   *icon_theme;
  const gchar    *icon_name;
  ExoHelper      *helper;
  GMainLoop      *loop;
  GdkCursor      *cursor;
  GdkPixbuf      *icon;
  GtkWidget      *image;
  GtkWidget      *menu;
  GtkWidget      *item;
  GList          *helpers;
  GList          *lp;
  gint            icon_size;

  g_return_if_fail (EXO_IS_HELPER_CHOOSER (chooser));
  g_return_if_fail (GTK_IS_BUTTON (button));

  /* set a watch cursor while loading the menu */
  if (G_LIKELY (button->window != NULL))
    {
      cursor = gdk_cursor_new (GDK_WATCH);
      gdk_window_set_cursor (button->window, cursor);
      gdk_cursor_unref (cursor);
      gdk_flush ();
    }

  /* allocate a new menu */
  menu = gtk_menu_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (menu));
  gtk_menu_set_screen (GTK_MENU (menu), gtk_widget_get_screen (button));

  /* set Atk popup-window relation for the menu */
  object = gtk_widget_get_accessible (button);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (menu));
  relation = atk_relation_new (&object, 1, ATK_RELATION_POPUP_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  /* determine the icon theme to use */
  icon_theme = gtk_icon_theme_get_default ();

  /* determine the menu icon size */
  gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &icon_size, &icon_size);

  /* append menu items for all available helpers */
  helpers = exo_helper_database_get_all (chooser->database, chooser->category);
  for (lp = helpers; lp != NULL; lp = lp->next)
    {
      /* determine the helper */
      helper = EXO_HELPER (lp->data);
      
      /* add a menu item for the helper */
      item = gtk_image_menu_item_new_with_label (exo_helper_get_name (helper));
      g_object_set_data_full (G_OBJECT (item), I_("exo-helper"), helper, g_object_unref);
      g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (menu_activate), chooser);
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      gtk_widget_show (item);

      /* try to load the icon for the helper */
      icon_name = exo_helper_get_icon (helper);
      if (G_LIKELY (icon_name != NULL))
        {
          /* load the icon */
          if (g_path_is_absolute (icon_name))
            icon = gdk_pixbuf_new_from_file_at_scale (icon_name, icon_size, icon_size, TRUE, NULL);
          else
            icon = gtk_icon_theme_load_icon (icon_theme, icon_name, icon_size, 0, NULL);

          /* setup the icon */
          if (G_LIKELY (icon != NULL))
            {
              image = gtk_image_new_from_pixbuf (icon);
              gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image);
              g_object_unref (G_OBJECT (icon));
              gtk_widget_show (image);
            }
        }
    }

  if (G_LIKELY (helpers != NULL))
    {
      item = gtk_separator_menu_item_new ();
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      gtk_widget_show (item);
    }

  item = gtk_menu_item_new_with_mnemonic (_("_Other..."));
  gtk_tooltips_set_tip (chooser->tooltips, item, _("Use a custom application which is not included in the above list."), NULL);
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (menu_activate_other), chooser);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  gtk_widget_show (item);

  /* make sure the menu has atleast the same width as the chooser */
  if (menu->allocation.width < GTK_WIDGET (chooser)->allocation.width)
    gtk_widget_set_size_request (menu, GTK_WIDGET (chooser)->allocation.width, -1);

  /* reset the watch cursor on the chooser */
  if (G_LIKELY (button->window != NULL))
    gdk_window_set_cursor (button->window, NULL);

  /* allocate a new main loop */
  loop = g_main_loop_new (NULL, FALSE);
  g_signal_connect_swapped (G_OBJECT (menu), "deactivate", G_CALLBACK (g_main_loop_quit), loop);

  /* run the loop for the menu */
  gtk_grab_add (menu);
  gtk_menu_popup (GTK_MENU (menu), NULL, NULL, menu_position, button, 0, gtk_get_current_event_time ());
  g_main_loop_run (loop);
  gtk_grab_remove (menu);
  g_main_loop_unref (loop);

  gtk_button_released (GTK_BUTTON (button));
  g_object_unref (G_OBJECT (menu));
}



/**
 * exo_helper_chooser_new:
 * @category : the #ExoHelperCategory for the chooser.
 *
 * Allocates a new #ExoHelperChooser for the given
 * @category.
 *
 * Return value: the newly allocated #ExoHelperChooser.
 **/
GtkWidget*
exo_helper_chooser_new (ExoHelperCategory category)
{
  g_return_val_if_fail (category >= 0 && category < EXO_HELPER_N_CATEGORIES, NULL);
  return g_object_new (EXO_TYPE_HELPER_CHOOSER, "category", category, NULL);
}



/**
 * exo_helper_chooser_get_category:
 * @chooser : an #ExoHelperChooser.
 *
 * Returns the #ExoHelperCategory which is configured
 * by @chooser.
 *
 * Return value: the category for @chooser.
 **/
ExoHelperCategory
exo_helper_chooser_get_category (const ExoHelperChooser *chooser)
{
  g_return_val_if_fail (EXO_IS_HELPER_CHOOSER (chooser), EXO_HELPER_WEBBROWSER);
  return chooser->category;
}



/**
 * exo_helper_chooser_set_category:
 * @chooser  : an #ExoHelperChooser.
 * @category : an #ExoHelperCategory.
 *
 * Sets the category for @chooser to @category.
 **/
void
exo_helper_chooser_set_category (ExoHelperChooser *chooser,
                                 ExoHelperCategory category)
{
  g_return_if_fail (EXO_IS_HELPER_CHOOSER (chooser));
  g_return_if_fail (category >= 0 && category < EXO_HELPER_N_CATEGORIES);

  /* apply the new category */
  chooser->category = category;
  exo_helper_chooser_update (chooser);
  g_object_notify (G_OBJECT (chooser), "category");
}



/**
 * exo_helper_chooser_get_is_valid:
 * @chooser : an #ExoHelperChooser.
 *
 * Returns %TRUE if a valid helper is selected for
 * @chooser.
 *
 * Return value: %TRUE if a valid helper is selected.
 **/
gboolean
exo_helper_chooser_get_is_valid (const ExoHelperChooser *chooser)
{
  g_return_val_if_fail (EXO_IS_HELPER_CHOOSER (chooser), FALSE);
  return chooser->is_valid;
}
