/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <glib.h>



/* --- externals --- */
extern unsigned data_bin_length;
extern const char data_bin[];
extern unsigned data_txt_length;
extern const char data_txt[];



static void
verify (const gchar *path,
        const gchar *data,
        unsigned     data_length)
{
  const guint8 *dp;
  const guint8 *cp;
  gchar        *contents;
  gsize         contents_length;
  gsize         n;

  g_file_get_contents (path, &contents, &contents_length, NULL);

  if (data_length != contents_length)
    {
      g_warning ("Lengths for \"%s\" differ: data_length=%u, contents_length=%u",
                 path, (guint) data_length, (guint) contents_length);
    }
  else
    {
      dp = (const guint8 *) data;
      cp = (const guint8 *) contents;

      for (n = 0; n < contents_length; ++n)
        if (dp[n] != cp[n])
          {
            g_warning ("Contents for \"%s\" differ: data[%u]=0x%02x, contents[%u]=0x%02x",
                       path, (guint) n, (guint) dp[n], (guint) n, (guint) cp[n]);
            break;
          }
    }

  g_free (contents);
}



int
main (int argc, char **argv)
{
  verify ("data/data.bin", data_bin, data_bin_length);
  verify ("data/data.txt", data_txt, data_txt_length);

  return EXIT_SUCCESS;
}
