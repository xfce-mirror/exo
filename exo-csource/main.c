/*-
 * Copyright (c) 2005-2007 Benedikt Meurer <benny@xfce.org>
 * Copyright (c) 2007      Nick Schermer <nick@xfce.org>
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

#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>



/* --- prototypes --- */
static void parse_args    (gint        *argc_p,
                           gchar     ***argv_p);
static void print_csource (FILE        *fp,
                           const gchar *data,
                           gsize        length,
                           const gchar *filename);
static void print_usage   (void);
static void print_version (void);



/* --- variables --- */
static gboolean     gen_buildlist = FALSE;
static gboolean     gen_stripcomments = FALSE;
static gboolean     gen_stripcontent = FALSE;
static const gchar *gen_linkage = "static ";
static const gchar *gen_varname = "my_data";
static const gchar *out_name = NULL;



static void
parse_args (gint    *argc_p,
            gchar ***argv_p)
{
  gchar **argv = *argv_p;
  gchar  *s;
  gint    argc = *argc_p;
  gint    n;
  gint    m;

  for (n = 1; n < argc; ++n)
    {
      if (strcmp (argv[n], "--help") == 0
          || strcmp (argv[n], "-h") == 0)
        {
          print_usage ();
          exit (EXIT_SUCCESS);
        }
      else if (strcmp (argv[n], "--version") == 0
          || strcmp (argv[n], "-V") == 0)
        {
          print_version ();
          exit (EXIT_SUCCESS);
        }
      else if (strcmp (argv[n], "--extern") == 0)
        {
          gen_linkage = "";
          argv[n] = NULL;
        }
      else if (strcmp (argv[n], "--static") == 0)
        {
          gen_linkage = "static ";
          argv[n] = NULL;
        }
      else if (strncmp (argv[n], "--name=", 7) == 0
          || strcmp (argv[n], "--name") == 0)
        {
          s = argv[n] + 6;

          if (G_LIKELY (*s == '='))
            {
              gen_varname = g_strdup (s + 1);
            }
          else if (n + 1 < argc)
            {
              gen_varname = g_strdup (argv[n + 1]);
              argv[n++] = NULL;
            }

          argv[n] = NULL;
        }
      else if (strcmp (argv[n], "--build-list") == 0)
        {
          gen_buildlist = TRUE;
          argv[n] = NULL;
        }
      else if (strcmp (argv[n], "--strip-comments") == 0)
        {
          gen_stripcomments = TRUE;
          argv[n] = NULL;
        }
      else if (strcmp (argv[n], "--strip-content") == 0)
        {
          gen_stripcontent = TRUE;
          argv[n] = NULL;
        }
      else if (strcmp (argv[n], "--output") == 0
        || strncmp (argv[n], "--output=", 9) == 0)
        {
          s = argv[n] + 8;

          if (G_LIKELY (*s == '='))
            {
              out_name = g_strdup (s + 1);
            }
          else if (n + 1 < argc)
            {
              out_name = g_strdup (argv[n + 1]);
              argv[n++] = NULL;
            }

          argv[n] = NULL;
        }
    }

  for (m = 0, n = 1; n < argc; ++n)
    {
      if (m > 0)
        {
          if (argv[n] != NULL)
            {
              argv[m++] = argv[n];
              argv[n] = NULL;
            }
        }
      else if (argv[n] == NULL)
        {
          m = n;
        }
    }

  if (m > 0)
    *argc_p = m;
}



static void
print_csource (FILE        *fp,
               const gchar *data,
               gsize        length,
               const gchar *filename)
{
  const guint8 *p = (const guint8 *) data;
  gboolean      pad = FALSE;
  gboolean      inside_comment = FALSE;
  gboolean      inside_content = TRUE;
  gint          column = 0;
  guint         real_length = 0;

  g_fprintf (fp, "/* automatically generated from %s */\n", filename);
  g_fprintf (fp, "#ifdef __SUNPRO_C\n");
  g_fprintf (fp, "#pragma align 4 (%s)\n", gen_varname);
  g_fprintf (fp, "#endif\n");
  g_fprintf (fp, "#ifdef __GNUC__\n");
  g_fprintf (fp, "%sconst char %s[] __attribute__ ((__aligned__ (4))) =\n", gen_linkage, gen_varname);
  g_fprintf (fp, "#else\n");
  g_fprintf (fp, "%sconst char %s[] =\n", gen_linkage, gen_varname);
  g_fprintf (fp, "#endif\n");
  g_fprintf (fp, "{\n");
  g_fprintf (fp, "  \"");

  for (; length-- > 0; ++p)
    {
      if (column > 70)
        {
          g_fprintf (fp, "\"\n  \"");
          column = 0;
        }

      /* strip XML/HTML comments */
      if (gen_stripcomments)
        {
          /* skip comments */
          if (length >= 4 && p[0] == '<' && p[1] == '!' && p[2] == '-' && p[3] == '-')
            {
              inside_comment = TRUE;
              length -= 3;
              p += 3;
              continue;
            }
          else if (inside_comment)
            {
              /* check for end of comment */
              if (length >= 3 && p[0] == '-' && p[1] == '-' && p[2] == '>')
                {
                  inside_comment = FALSE;
                  length -= 2;
                  p += 2;
                }
              continue;
            }
        }

      /* strip XML content (the stuff between the nodes) */
      if (gen_stripcontent)
        {
          if (!inside_content && *p == '>')
            inside_content = TRUE;
          else if (inside_content && *p == '<')
            inside_content = FALSE;
          else if (inside_content && !g_ascii_isspace (*p))
            inside_content = FALSE;
          else if (inside_content)
            continue;
        }

      if (*p == '\"')
        {
          column += g_fprintf (fp, "\\\"");
          pad = FALSE;
        }
      else if (*p == '\'')
        {
          column += g_fprintf (fp, "\\\'");
          pad = FALSE;
        }
      else if (*p == '\\')
        {
          column += g_fprintf (fp, "\\\\");
          pad = FALSE;
        }
      else if (*p == '\n')
        {
          column += g_fprintf (fp, "\\n");
          pad = FALSE;
        }
      else if (*p == '\r')
        {
          column += g_fprintf (fp, "\\r");
          pad = FALSE;
        }
      else if (*p == '\t')
        {
          column += g_fprintf (fp, "\\t");
          pad = FALSE;
        }
      else if (g_ascii_isprint (*p))
        {
          if (pad && g_ascii_isdigit (*p))
            column += g_fprintf (fp, "\"\"");
          column += g_fprintf (fp, "%c", *p);
          pad = FALSE;
        }
      else
        {
          column += g_fprintf (fp, "\\%03o", (guint) *p);
          pad = TRUE;
        }

      real_length++;
    }

  g_fprintf (fp, "\"\n};\n\n");
  g_fprintf (fp, "%sconst unsigned %s_length = %uu;\n\n", gen_linkage, gen_varname, real_length);
}



static void
print_usage (void)
{
  g_print (_("Usage: %s [options] [file]\n"), g_get_prgname ());
  g_print (_("       %s [options] --build-list [[name file]...]\n"), g_get_prgname ());
  g_print ("\n");
  g_print (_("  -h, --help        Print this help message and exit\n"));
  g_print (_("  -V, --version     Print version information and exit\n"));
  g_print (_("  --extern          Generate extern symbols\n"));
  g_print (_("  --static          Generate static symbols\n"));
  g_print (_("  --name=identifier C macro/variable name\n"));
  g_print (_("  --build-list      Parse (name, file) pairs\n"));
  g_print (_("  --strip-comments  Remove comments from XML files\n"));
  g_print (_("  --strip-content   Remove node contents from XML files\n"));
  g_print (_("  --output=filename Write generated csource to specified file\n"));
  g_print ("\n");
}



static void
print_version (void)
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



int
main (int argc, char **argv)
{
  gboolean toggle = FALSE;
  GError  *error = NULL;
  gchar  **p;
  gchar   *filename;
  gchar   *data;
  gsize    length;
  gint     n;
  FILE    *out_file;

  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);

#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

  textdomain (GETTEXT_PACKAGE);
  setlocale (LC_ALL, NULL);

  /* set program name */
  g_set_prgname (g_path_get_basename (argv[0]));

  /* parse command line arguments */
  parse_args (&argc, &argv);

  if (!gen_buildlist)
    {
      if (G_UNLIKELY (argc != 2))
        {
          print_usage ();
          return EXIT_FAILURE;
        }

      if (out_name == NULL)
          out_file = stdout;
      else
        {
          out_file = g_fopen (out_name, "w");
          if (out_file == NULL) {
            g_fprintf (stderr, "Failed to open output file \"%s\"\n", out_name);
            return EXIT_FAILURE;
          }
        }

#ifdef G_OS_WIN32
      filename = g_local_to_utf8 (argv[1], -1, NULL, NULL, NULL);
#else
      filename = argv[1];
#endif

      if (!g_file_get_contents (filename, &data, &length, &error))
        {
          g_fprintf (stderr, "%s: Failed to load \"%s\": %s\n",
                     g_get_prgname (), filename, error->message);
          g_error_free (error);
          return EXIT_FAILURE;
        }

      print_csource (out_file, data, length, filename);

      if (out_file != NULL && out_file != stdout)
        fclose (out_file);

      g_free (data);
    }
  else
    {
      for (n = argc - 1, p = argv + 1; n-- > 0; ++p, toggle = !toggle)
        {
#ifdef G_OS_WIN32
          filename = g_local_to_utf8 (*p, -1, NULL, NULL, NULL);
#else
          filename = *p;
#endif

          if (!toggle)
            {
              gen_varname = filename;
            }
          else
            {
              if (!g_file_get_contents (filename, &data, &length, &error))
                {
                  g_fprintf (stderr, "%s: Failed to load \"%s\": %s\n",
                             g_get_prgname (), filename, error->message);
                  g_error_free (error);
                  return EXIT_FAILURE;
                }

              print_csource (stdout, data, length, filename);

              g_free (data);
            }
        }
    }

  return EXIT_SUCCESS;
}
