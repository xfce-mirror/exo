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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo-helper/exo-helper-chooser-dialog.h>
#include <exo-helper/exo-helper-launcher-dialog.h>
#include <exo-helper/exo-helper-utils.h>



static const gchar *CATEGORY_EXEC_ERRORS[] =
{
  N_("Failed to execute default Web Browser"),
  N_("Failed to execute default Mail Reader"),
  N_("Failed to execute default Terminal Emulator"),
};



static void
usage (void)
{
  g_print ("%s\n", _("Usage: exo-helper [OPTION...]"));
  g_print ("\n");
  g_print ("%s\n", _("  -h, --help                          Print this help message and exit"));
  g_print ("%s\n", _("  -v, --version                       Print version information and exit"));
  g_print ("\n");
  g_print ("%s\n", _("  --configure                         Open the Preferred Applications\n"
                     "                                      configuration dialog"));
  g_print ("\n");
  g_print ("%s\n", _("  --launch TYPE [PARAMETER]           Launch the default helper of TYPE\n"
                     "                                      with the optional PARAMETER, where\n"
                     "                                      TYPE is one of the following values."));
  g_print ("\n");
  g_print ("%s\n", _("The following TYPEs are supported for the --launch command:"));
  g_print ("\n");

  /* Note to Translators: Do not translate the TYPEs (WebBrowser, MailReader, TerminalEmulator),
   * since the exo-helper utility will not accept localized TYPEs.
   */
  g_print ("%s\n", _("  WebBrowser       - The preferred Web Browser.\n"
                     "  MailReader       - The preferred Mail Reader.\n"
                     "  TerminalEmulator - The preferred Terminal Emulator."));
  g_print ("\n");
}



int
main (int argc, char **argv)
{
  ExoHelperCategory  category;
  ExoHelperDatabase *database;
  ExoHelper         *helper;
  GtkWidget         *dialog;
  GError            *error = NULL;
  gint               result = EXIT_SUCCESS;

  /* sanity check helper categories */
  g_assert (EXO_HELPER_N_CATEGORIES == G_N_ELEMENTS (CATEGORY_EXEC_ERRORS));

#ifdef GETTEXT_PACKAGE
  /* setup i18n support */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
#endif

  /* initialize Gtk+ */
  gtk_init (&argc, &argv);

  /* setup default window icon */
  gtk_window_set_default_icon_name ("preferences-desktop-default-applications");

  /* check for the action to perform */
  if (argc == 2 && strcmp (argv[1], "--configure") == 0)
    {
      dialog = exo_helper_chooser_dialog_new ();
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
    }
  else if ((argc == 3 || argc == 4) && strcmp (argv[1], "--launch") == 0)
    {
      /* try to parse the type */
      if (!exo_helper_category_from_string (argv[2], &category))
        {
          g_warning (_("Invalid helper type `%s'"), argv[2]);
          return EXIT_FAILURE;
        }

      /* determine the default helper for the category */
      database = exo_helper_database_get ();
      helper = exo_helper_database_get_default (database, category);

      /* check if we have a valid helper */
      if (G_UNLIKELY (helper == NULL))
        {
          /* ask the user to choose a default helper for category */
          dialog = exo_helper_launcher_dialog_new (category);
          if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
            helper = exo_helper_database_get_default (database, category);
          gtk_widget_destroy (dialog);
        }

      /* release our reference on the database */
      g_object_unref (G_OBJECT (database));

      /* check if we have a valid helper now */
      if (G_LIKELY (helper != NULL))
        {
          /* try to execute the helper with the given parameter */
          if (!exo_helper_execute (helper, NULL, (argc > 3) ? argv[3] : NULL, &error))
            {
              dialog = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                               "%s.", _(CATEGORY_EXEC_ERRORS[category]));
              gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", error->message);
              gtk_dialog_run (GTK_DIALOG (dialog));
              gtk_widget_destroy (dialog);
              g_error_free (error);
              result = EXIT_FAILURE;
            }
          g_object_unref (G_OBJECT (helper));
        }
    }
  else if (argc == 2 && strcmp (argv[1], "--help") == 0)
    {
      usage ();
    }
  else if (argc == 2 && strcmp (argv[1], "--version") == 0)
    {
      g_print (_("%s (Xfce %s)\n\n"
                 "Copyright (c) 2003-2006\n"
                 "        os-cillation e.K. All rights reserved.\n\n"
                 "Written by Benedikt Meurer <benny@xfce.org>.\n\n"
                 "Built with Gtk+-%d.%d.%d, running Gtk+-%d.%d.%d.\n\n"
                 "Please report bugs to <%s>.\n"),
                 PACKAGE_STRING, xfce_version_string (),
                 GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION,
                 gtk_major_version, gtk_minor_version, gtk_micro_version,
                 PACKAGE_BUGREPORT);
    }
  else
    {
      result = EXIT_FAILURE;
      usage ();
    }

  return result;
}
