/* $Id$ */
/*-
 * Copyright (c) 2004  Benedikt Meurer <benny@xfce.org>
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



/**
 * exo_str_is_equal:
 * @a           : pointer to first string or %NULL.
 * @b           : pointer to second string or %NULL.
 *
 * %NULL-safe string comparison. Returns true if both @a and @b are
 * %NULL or if @a and @b refer to valid strings which are equal.
 *
 * Return value : %TRUE if @a equals @b, else %FALSE.
 **/
gboolean
exo_str_is_equal (const gchar *a, const gchar *b)
{
  if (a == NULL && b == NULL)
    return TRUE;
  else if (a == NULL || b == NULL)
    return FALSE;
  else
    return (strcmp (a, b) == 0);
}




