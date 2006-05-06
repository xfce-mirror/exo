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

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <stdio.h>

#include <exo-desktop-item-edit/exo-die-utils.h>



/**
 * exo_die_g_key_file_set_locale_value:
 * @key_file : the #GKeyFile.
 * @group    : the group name.
 * @key      : the key name.
 * @value    : the new value.
 *
 * Stores @value localized for @key in @group if it
 * is already localized.
 **/
void
exo_die_g_key_file_set_locale_value (GKeyFile    *key_file,
                                     const gchar *group,
                                     const gchar *key,
                                     const gchar *value)
{
  const gchar * const *locale;
  gchar               *name;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group != NULL);
  g_return_if_fail (key != NULL);
  g_return_if_fail (value != NULL);

  /* try localized first */
  for (locale = g_get_language_names (); *locale != NULL; ++locale)
    {
      name = g_strdup_printf ("%s[%s]", key, *locale);
      if (g_key_file_has_key (key_file, group, name, NULL))
        {
          g_key_file_set_string (key_file, group, name, value);
          g_free (name);
          break;
        }
    }

  /* fallback to unlocalized */
  if (G_UNLIKELY (*locale == NULL))
    g_key_file_set_string (key_file, group, key, value);
}



/**
 * exo_die_g_key_file_save:
 * @key_file : the #GKeyFile.
 * @create   : whether to create.
 * @base     : file or folder (if @create).
 * @error    : return location for errors or %NULL.
 *
 * Return value: %TRUE if successfull, %FALSE otherwise.
 **/
gboolean
exo_die_g_key_file_save (GKeyFile    *key_file,
                         gboolean     create,
                         const gchar *base,
                         GError     **error)
{
  gsize  data_length;
  gchar *filename;
  gchar *data;
  gchar *name;
  gchar *s;
  FILE  *fp;
  gint   n;

  g_return_val_if_fail (base != NULL, FALSE);
  g_return_val_if_fail (key_file != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* check if we should create a new file */
  if (G_LIKELY (create))
    {
      /* check base */
      if (g_file_test (base, G_FILE_TEST_IS_REGULAR))
        {
          /* we can override an existing file */
          filename = g_strdup (base);
        }
      else if (g_file_test (base, G_FILE_TEST_IS_DIR))
        {
          /* determine the desktop entry name */
          name = g_key_file_get_locale_string (key_file, "Desktop Entry", "Name", NULL, NULL);
          if (G_UNLIKELY (name == NULL))
            name = g_strdup ("launcher");

          /* replace invalid file system characters */
          for (s = name; *s != '\0'; ++s)
            if (G_IS_DIR_SEPARATOR (*s) || *s == '.')
              *s = '_';

          /* try to transform name to local encoding */
          s = g_filename_from_utf8 (name, -1, NULL, NULL, NULL);
          if (G_LIKELY (s != NULL))
            {
              /* use the local name */
              g_free (name);
              name = s;
            }

          /* try to come up with a unique file name */
          filename = g_strconcat (base, G_DIR_SEPARATOR_S, name, ".desktop", NULL);
          for (n = 0; g_file_test (filename, G_FILE_TEST_EXISTS); ++n)
            {
              /* release the previous name */
              g_free (filename);

              /* generate a new file name */
              filename = g_strdup_printf ("%s%s%s%d.desktop", base, G_DIR_SEPARATOR_S, name, n);
            }

          /* release the name */
          g_free (name);
        }
      else
        {
          /* base is not a directory, cannot save */
          g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (ENOTDIR), g_strerror (ENOTDIR));
          return FALSE;
        }
    }
  else
    {
      /* base is the filename */
      filename = g_strdup (base);
    }

  /* determine the data for the key file */
  data = g_key_file_to_data (key_file, &data_length, error);
  if (G_UNLIKELY (data == NULL))
    {
      g_free (filename);
      return FALSE;
    }

  /* try to open the file for writing */
  fp = fopen (filename, "w");
  if (G_UNLIKELY (fp == NULL))
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno), g_strerror (errno));
      g_free (filename);
      g_free (data);
      return FALSE;
    }

  /* try to write the data to the file */
  if (fwrite (data, data_length, 1, fp) != 1)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno), g_strerror (errno));
      g_free (filename);
      g_free (data);
      fclose (fp);
      return FALSE;
    }

  /* cleanup */
  g_free (filename);
  g_free (data);
  fclose (fp);

  return TRUE;
}




