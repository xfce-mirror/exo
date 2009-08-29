/* $Id$ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>.
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

#include <glib/gstdio.h>

#include <exo/exo.h>



#define MATCH_BROWSER "^(([^:/?#]+)://)?([^/?#])([^?#]*)(\\?([^#]*))?(#(.*))?"
#define MATCH_MAILER  "^[a-z0-9][a-z0-9_.-]*@[a-z0-9][a-z0-9-]*(\\.[a-z0-9][a-z0-9-]*)+$"



/**
 * For testing this code, the following commands should work:
 *
 * exo-open --launch WebBrowser http://xfce.org (bug #5461).
 * exo-open --launch WebBrowser http://xfce.org bugs.xfce.org 'http://www.google.com/search?q=what is a space' 'http://wiki.xfce.org'
 * exo-open http://xfce.org
 * exo-open --launch TerminalEmulator ./script.sh 'something with a space' 'nospace' (bug #5132).
 * exo-open --launch TerminalEmulator ssh -l username some.host.com
 * xfterm4 -e ssh -l ssh -l username some.host.com (bug #5301, this generates line below)
 *   exo-open --launch TerminalEmulator 'ssh -l username some.host.com'
 **/



static gboolean  opt_help = FALSE;
static gboolean  opt_version = FALSE;
static gchar    *opt_launch = NULL;
static gchar    *opt_working_directory = NULL;

static GOptionEntry entries[] =
{
  { "help", '?', 0, G_OPTION_ARG_NONE, &opt_help, NULL, NULL, },
  { "version", 'V', 0, G_OPTION_ARG_NONE, &opt_version, NULL, NULL, },
  { "launch", 0, 0, G_OPTION_ARG_STRING, &opt_launch, NULL, NULL, },
  { "working-directory", 0, 0, G_OPTION_ARG_FILENAME, &opt_working_directory, NULL, NULL, },
  { NULL, },
};



static void
usage (void)
{
  g_print ("%s\n", _("Usage: exo-open [URLs...]"));
  g_print ("%s\n", _("       exo-open --launch TYPE [PARAMETERs...]"));
  g_print ("\n");
  g_print ("%s\n", _("  -?, --help                          Print this help message and exit"));
  g_print ("%s\n", _("  -V, --version                       Print version information and exit"));
  g_print ("\n");
  g_print ("%s\n", _("  --launch TYPE [PARAMETERs...]       Launch the preferred application of\n"
                     "                                      TYPE with the optional PARAMETERs, where\n"
                     "                                      TYPE is one of the following values."));
  g_print ("\n");
  g_print ("%s\n", _("  --working-directory DIRECTORY       Default working directory for applications\n"
                     "                                      when using the --launch option."));
  g_print ("\n");
  g_print ("%s\n", _("The following TYPEs are supported for the --launch command:"));
  g_print ("\n");

  /* Note to Translators: Do not translate the TYPEs (WebBrowser, MailReader, TerminalEmulator),
   * since the exo-helper utility will not accept localized TYPEs.
   */
  g_print ("%s\n", _("  WebBrowser       - The preferred Web Browser.\n"
                     "  MailReader       - The preferred Mail Reader.\n"
                     "  FileManager      - The preferred File Manager.\n"
                     "  TerminalEmulator - The preferred Terminal Emulator."));
  g_print ("\n");
  g_print ("%s\n", _("If you don't specify the --launch option, exo-open will open all specified\n"
                     "URLs with their preferred URL handlers. Else, if you specify the --launch\n"
                     "option, you can select which preferred application you want to run, and\n"
                     "pass additional parameters to the application (i.e. for TerminalEmulator\n"
                     "you can pass the command line that should be run in the terminal)."));
  g_print ("\n");
}



static gboolean
exo_open_looks_like_an_uri (const gchar *string)
{
  const gchar *s = string;

  /* <scheme> starts with an alpha character */
  if (g_ascii_isalpha (*s))
    {
      /* <scheme> continues with (alpha | digit | "+" | "-" | ".")* */
      for (++s; g_ascii_isalnum (*s) || *s == '+' || *s == '-' || *s == '.'; ++s);

      /* <scheme> must be followed by ":" */
      return (*s == ':');
    }

  return FALSE;
}



static const gchar *
exo_open_find_scheme (const gchar *string)
{
  gboolean  exists;
  gchar    *current_dir, *path;

  /* is an absolute path, return file uri */
  if (g_path_is_absolute (string))
    return "file://";

  /* treat it like a relative path */
  current_dir = g_get_current_dir ();
  path = g_build_filename (current_dir, string, NULL);
  g_free (current_dir);

  /* verify that a file of the given name exists */
  exists = g_file_test (path, G_FILE_TEST_EXISTS);
  g_free (path);
  if (exists)
    return "file://";

  /* regular expression to check if it looks like an email address */
  if (g_regex_match_simple (MATCH_MAILER, string, G_REGEX_CASELESS, 0))
    return "mailto:";

  /* regular expression to check if it looks like an url */
  if (g_regex_match_simple (MATCH_BROWSER, string, G_REGEX_CASELESS, 0))
    return "http://";

  return NULL;
}



int
main (int argc, char **argv)
{
  GOptionContext *context;
  GtkWidget      *dialog;
  GError         *err = NULL;
  gchar          *parameter, *quoted;
  gint            result = EXIT_SUCCESS;
  GString        *join;
  guint           i;
  gchar          *uri;
  const gchar    *scheme;

#ifdef GETTEXT_PACKAGE
  /* setup i18n support */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
#endif

  /* try to parse the command line parameters */
  context = g_option_context_new (NULL);
  g_option_context_set_help_enabled (context, FALSE);
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_set_ignore_unknown_options (context, TRUE);
  if (!g_option_context_parse (context, &argc, &argv, &err))
    {
      g_fprintf (stderr, "exo-open: %s.\n", err->message);
      g_error_free (err);
      return EXIT_FAILURE;
    }

  /* setup default icon for windows */
  gtk_window_set_default_icon_name ("preferences-desktop-default-applications");

  /* check what to do */
  if (G_UNLIKELY (opt_help))
    {
      usage ();
    }
  else if (G_UNLIKELY (opt_version))
    {
      g_print ("%s %s\n\n", g_get_prgname (), PACKAGE_VERSION);
      g_print (_("Copyright (c) %s\n"
                 "        os-cillation e.K. All rights reserved.\n\n"
                 "Written by Benedikt Meurer <benny@xfce.org>.\n\n"),
               "2005-2007");
      g_print (_("%s comes with ABSOLUTELY NO WARRANTY,\n"
                 "You may redistribute copies of %s under the terms of\n"
                 "the GNU Lesser General Public License which can be found in the\n"
                 "%s source package.\n\n"), g_get_prgname (), g_get_prgname (), PACKAGE_TARNAME);
      g_print (_("Please report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
    }
  else if (G_LIKELY (opt_launch != NULL))
    {
      if (argc > 1)
        {
          /* NOTE: see the comment at the top of this document! */

          /* combine all specified parameters to one parameter string */
          join = g_string_new (NULL);
          for (i = 1; argv[i] != NULL; i++)
            {
              /* separate the arguments */
              if (i > 1)
                join = g_string_append_c (join, ' ');

              /* only quote arguments with spaces if there are multiple
               * arguments to be merged, this is a bit of magic to make
               * common cares work property, see sample above with xfrun4 */
              if (argc > 2 && strchr (argv[i], ' ') != NULL)
                {
                  quoted = g_shell_quote (argv[i]);
                  join = g_string_append (join, quoted);
                  g_free (quoted);
                }
              else
                {
                  join = g_string_append (join, argv[i]);
                }
            }
          parameter = g_string_free (join, FALSE);
        }
      else
        {
          parameter = NULL;
        }

#ifndef NDEBUG
      g_message ("launch=%s, wd=%s, parameters (%d)=%s", opt_launch, opt_working_directory, argc, parameter);
#endif

      /* run the preferred application */
      if (!exo_execute_preferred_application (opt_launch, parameter, opt_working_directory, NULL, &err))
        {
          /* display an error dialog */
          dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                           _("Failed to launch preferred application for category \"%s\"."),
                                           opt_launch);
          gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", err->message);
          gtk_dialog_run (GTK_DIALOG (dialog));
          gtk_widget_destroy (dialog);
          result = EXIT_FAILURE;
          g_error_free (err);
        }

      /* cleanup */
      g_free (parameter);
    }
  else if (argc > 1)
    {
      /* open all specified urls */
      for (argv += 1; result == EXIT_SUCCESS && *argv != NULL; ++argv)
        {
          if (exo_open_looks_like_an_uri (*argv))
            {
              /* use the argument directly */
              uri = g_strdup (*argv);
            }
          else
            {
              /* try to find a valid scheme */
              scheme = exo_open_find_scheme (*argv);
              if (G_LIKELY (scheme != NULL))
                uri = g_strconcat (scheme, *argv, NULL);
              else
                uri = NULL;
            }

          if (uri == NULL)
            {
              /* display an error dialog */
              dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                               _("Unable to detect the URI-scheme of \"%s\"."), *argv);
              gtk_dialog_run (GTK_DIALOG (dialog));
              gtk_widget_destroy (dialog);

              result = EXIT_FAILURE;
            }
          else if (!gtk_show_uri (NULL, uri, 0, &err))
            {
              /* display an error dialog */
              dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                               _("Failed to open URI \"%s\"."), uri);
              gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", err->message);
              g_error_free (err);
              gtk_dialog_run (GTK_DIALOG (dialog));
              gtk_widget_destroy (dialog);

              result = EXIT_FAILURE;
            }

          /* cleanup */
          g_free (uri);
        }
    }
  else
    {
      result = EXIT_FAILURE;
      usage ();
    }

  return result;
}
