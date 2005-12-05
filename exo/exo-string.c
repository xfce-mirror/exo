/* $Id$ */
/*-
 * Copyright (c) 2004-2005 os-cillation e.K.
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo-string.h>
#include <exo/exo-alias.h>



/**
 * exo_str_elide_underscores:
 * @text : A zero terminated string.
 *
 * Returns a copy of @text with all mnemonic underscores
 * stripped off.
 *
 * Return value: A copy of @text without underscores. The
 *               returned string must be freed when no
 *               longer required.
 **/
gchar*
exo_str_elide_underscores (const gchar *text)
{
  const gchar *s;
  gboolean     last_underscore = FALSE;
  gchar       *result;
  gchar       *t;

  g_return_val_if_fail (text != NULL, NULL);
  
  result = g_malloc (strlen (text) + 1);

  for (s = text, t = result; *s != '\0'; ++s)
    if (!last_underscore && *s == '_')
      {
        last_underscore = TRUE;
      }
    else
      {
        last_underscore = FALSE;
        *t++ = *s;
      }

  *t = '\0';

  return result;
}



/**
 * exo_str_is_equal:
 * @a : A pointer to first string or %NULL.
 * @b : A pointer to second string or %NULL.
 *
 * %NULL-safe string comparison. Returns %TRUE if both @a and @b are
 * %NULL or if @a and @b refer to valid strings which are equal.
 *
 * You should always prefer this function over strcmp().
 *
 * Return value: %TRUE if @a equals @b, else %FALSE.
 **/
gboolean
exo_str_is_equal (const gchar *a,
                  const gchar *b)
{
  if (a == NULL && b == NULL)
    return TRUE;
  else if (a == NULL || b == NULL)
    return FALSE;

  while (*a == *b++)
    if (*a++ == '\0')
      return TRUE;

  return FALSE;
}



/**
 * exo_str_replace:
 * @str         : the input string.
 * @pattern     : a search pattern in @str.
 * @replacement : replacement string for @pattern.
 *
 * Searches @str for occurances of @pattern and replaces each
 * such occurance with @replacement. Returns a newly allocated
 * copy of @str on which the given replacement were performed.
 * The caller is responsible to free the returned string using
 * g_free() when no longer needed.
 *
 * Note that @pattern and @replacement don't need to be of the
 * same size.
 *
 * Return value: a newly allocated copy of @str where all
 *               occurances of @pattern are replaced with
 *               @replacement.
 *
 * Since: 0.3.1.1
 **/
gchar*
exo_str_replace (const gchar *str,
                 const gchar *pattern,
                 const gchar *replacement)
{
  const gchar *s, *p;
  GString     *result;

  g_return_val_if_fail (str != NULL, NULL);
  g_return_val_if_fail (pattern != NULL, NULL);
  g_return_val_if_fail (replacement != NULL, NULL);

  /* empty patterns are kinda useless, so we just return a copy of str */
  if (G_UNLIKELY (*pattern == '\0'))
    return g_strdup (str);

  /* allocate the result string */
  result = g_string_new (NULL);

  /* process the input string */
  while (*str != '\0')
    {
      if (G_UNLIKELY (*str == *pattern))
        {
          /* compare the pattern to the current string */
          for (p = pattern + 1, s = str + 1; *p == *s; ++s, ++p)
            if (*p == '\0' || *s == '\0')
              break;

          /* check if the pattern matches */
          if (G_LIKELY (*p == '\0'))
            {
              g_string_append (result, replacement);
              str = s;
              continue;
            }
        }

      g_string_append_c (result, *str++);
    }

  return g_string_free (result, FALSE);
}



/**
 * exo_strndupv:
 * @strv  : String vector to duplicate.
 * @num   : Number of strings in @strv to
 *          duplicate.
 *
 * Creates a new string vector containing the
 * first @n elements of @strv.
 *
 * Return value: The new string vector. Should be
 *               freed using g_strfreev() when no
 *               longer needed.
 **/
gchar**
exo_strndupv (gchar **strv,
              gint    num)
{
  gchar **result;

  g_return_val_if_fail (strv != NULL, NULL);
  g_return_val_if_fail (num >= 0, NULL);

  result = g_new (gchar *, num + 1);
  result[num--] = NULL;
  for (; num >= 0; --num)
    result[num] = g_strdup (strv[num]);

  return result;
}



/**
 * exo_intern_string:
 * @string: a string
 * 
 * Returns a canonical representation for @string. Interned strings can
 * be compared for equality by comparing the pointers, instead of using strcmp()
 * or exo_str_is_equal(). exo_intern_string() takes a copy of the @string for its
 * internal usage, so @string does not need to be static.
 * 
 * Return value: a canonical representation for the string
 *
 * Since: 0.3.1.1
 */
G_CONST_RETURN gchar*
exo_intern_string (const gchar *string)
{
#if GLIB_CHECK_VERSION(2,9,0)
  return g_intern_string (string);
#else
  return (string != NULL) ? g_quark_to_string (g_quark_from_string (string)) : NULL;
#endif
}



/**
 * exo_intern_static_string:
 * @string: a static string
 * 
 * Returns a canonical representation for @string. Interned strings can
 * be compared for equality by comparing the pointers, instead of using strcmp()
 * or exo_str_is_equal(). exo_intern_static_string() does not copy the string,
 * therefore @string must not be freed or modified. 
 * 
 * Return value: a canonical representation for the string
 *
 * Since: 0.3.1.1
 */
G_CONST_RETURN gchar*
exo_intern_static_string (const gchar *string)
{
#if GLIB_CHECK_VERSION(2,9,0)
  return g_intern_static_string (string);
#else
  return (string != NULL) ? g_quark_to_string (g_quark_from_static_string (string)) : NULL;
#endif
}



#define __EXO_STRING_C__
#include <exo/exo-aliasdef.c>
