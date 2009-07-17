/* $Id$ */
/*-
 * Copyright (c) 2004-2007 os-cillation e.K.
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
  if (a == NULL || b == NULL)
    return (a == b);

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
 * exo_strdup_strftime:
 * @format : format string to pass to strftime(3). See the strftime(3) documentation
 *           for details.
 * @tm     : date/time, in struct tm format.
 *
 * Cover for standard date-and-time-formatting routine strftime that returns
 * a newly-allocated string of the correct size. The caller is responsible
 * to free the returned string using g_free() when no longer needed.
 *
 * Besides the buffer management, there are two differences between this
 * and the library strftime:
 *
 * The modifiers "-" and "_" between a "%" and a numeric directive
 * are defined as for the GNU version of strftime. "-" means "do not
 * pad the field" and "_" means "pad with spaces instead of zeroes".
 *
 * Non-ANSI extensions to strftime are flagged at runtime with a
 * warning, so it's easy to notice use of the extensions without
 * testing with multiple versions of the library.
 *
 * Return value: a newly allocated string containing the formatted date/time.
 *
 * Since: 0.3.3
 **/
gchar*
exo_strdup_strftime (const gchar     *format,
                     const struct tm *tm)
{
  static const gchar C_STANDARD_STRFTIME_CHARACTERS[] = "aAbBcdHIjmMpSUwWxXyYZ";
  static const gchar C_STANDARD_NUMERIC_STRFTIME_CHARACTERS[] = "dHIjmMSUwWyY";
  static const gchar SUS_EXTENDED_STRFTIME_MODIFIERS[] = "EO";
  const gchar       *remainder;
  const gchar       *percent;
  gboolean           strip_leading_zeros;
  gboolean           turn_leading_zeros_to_spaces;
  GString           *string;
  gsize              string_length;
  gchar              code[4];
  gchar              buffer[512];
  gchar             *piece;
  gchar             *result;
  gchar             *converted;
  gchar              modifier;
  gint               i;

  /* Format could be translated, and contain UTF-8 chars,
   * so convert to locale encoding which strftime uses.
   */
  converted = g_locale_from_utf8 (format, -1, NULL, NULL, NULL);
  if (G_UNLIKELY (converted == NULL))
    return NULL;

  /* start processing the format */
  string = g_string_new ("");
  remainder = converted;

  /* walk from % character to % character */
  for (;;)
    {
      /* look out for the next % character */
      percent = strchr (remainder, '%');
      if (percent == NULL)
        {
          /* we're done with the format */
          g_string_append (string, remainder);
          break;
        }

      /* append the characters in between */
      g_string_append_len (string, remainder, percent - remainder);

      /* handle the "%" character */
      remainder = percent + 1;
      switch (*remainder)
        {
        case '-':
          strip_leading_zeros = TRUE;
          turn_leading_zeros_to_spaces = FALSE;
          remainder++;
          break;

        case '_':
          strip_leading_zeros = FALSE;
          turn_leading_zeros_to_spaces = TRUE;
          remainder++;
          break;

        case '%':
          g_string_append_c (string, '%');
          remainder++;
          continue;

        case '\0':
          g_warning ("Trailing %% passed to exo_strdup_strftime");
          g_string_append_c (string, '%');
          continue;

        default:
          strip_leading_zeros = FALSE;
          turn_leading_zeros_to_spaces = FALSE;
          break;
        }

      modifier = 0;
      if (strchr (SUS_EXTENDED_STRFTIME_MODIFIERS, *remainder) != NULL)
        {
          modifier = *remainder++;
          if (*remainder == 0)
            {
              g_warning ("Unfinished %%%c modifier passed to exo_strdup_strftime", modifier);
              break;
            }
        }

      if (strchr (C_STANDARD_STRFTIME_CHARACTERS, *remainder) == NULL)
        g_warning ("exo_strdup_strftime does not support non-standard escape code %%%c", *remainder);

      /* convert code to strftime format. We have a fixed
       * limit here that each code can expand to a maximum
       * of 512 bytes, which is probably OK. There's no
       * limit on the total size of the result string.
       */
      i = 0;
      code[i++] = '%';
#ifdef HAVE_STRFTIME_EXTENSION
      if (modifier != 0)
        code[i++] = modifier;
#endif
      code[i++] = *remainder;
      code[i++] = '\0';
      string_length = strftime (buffer, sizeof (buffer), code, tm);
      if (string_length == 0)
        {
          /* we could put a warning here, but there's no
           * way to tell a successful conversion to
           * empty string from a failure
           */
          buffer[0] = '\0';
        }

      /* strip leading zeros if requested */
      piece = buffer;
      if (strip_leading_zeros || turn_leading_zeros_to_spaces)
        {
          if (strchr (C_STANDARD_NUMERIC_STRFTIME_CHARACTERS, *remainder) == NULL)
            g_warning ("exo_strdup_strftime does not support modifier for non-numeric escape code %%%c%c", remainder[-1], *remainder);

          if (*piece == '0')
            {
              while (*++piece == '\0') ;
              if (!g_ascii_isdigit (*piece))
                --piece;
            }

          if (turn_leading_zeros_to_spaces)
            {
              memset (buffer, ' ', piece - buffer);
              piece = buffer;
            }
        }

      /* advance */
      ++remainder;

      /* add this piece */
      g_string_append (string, piece);
    }

  /* Convert the string back into UTF-8 */
  result = g_locale_to_utf8 (string->str, -1, NULL, NULL, NULL);

  /* cleanup */
  g_string_free (string, TRUE);
  g_free (converted);

  return result;
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
