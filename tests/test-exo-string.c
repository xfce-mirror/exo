/* $Id$ */
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <exo/exo.h>



static const struct
{
  const gchar *str;
  const gchar *pattern;
  const gchar *replacement;
  const gchar *result;
} REPLACE_TESTS[] =
{
  /* Test empty pattern */
  { "Hello World", "", "World", "Hello World", },

  /* Test replace first word */
  { "Hello World", "Hello", "Holla", "Holla World", },

  /* Test replace last workd */
  { "Hello World", "World", "Dlrow", "Hello Dlrow", },

  /* Test multiple replacement */
  { "This is a test case for testing", "test", "toast", "This is a toast case for toasting", },
};



int
main (int argc, char **argv)
{
  gchar *result;
  guint  n;

  for (n = 0; n < G_N_ELEMENTS (REPLACE_TESTS); ++n)
    {
      result = exo_str_replace (REPLACE_TESTS[n].str, REPLACE_TESTS[n].pattern, REPLACE_TESTS[n].replacement);
      if (!exo_str_is_equal (result, REPLACE_TESTS[n].result))
        {
          g_print ("exo_str_replace(\"%s\",\"%s\",\"%s\") = \"%s\", but \"%s\" was expected\n",
                   REPLACE_TESTS[n].str, REPLACE_TESTS[n].pattern, REPLACE_TESTS[n].replacement,
                   result, REPLACE_TESTS[n].result);
        }
      g_free (result);
    }

  return EXIT_SUCCESS;
}
