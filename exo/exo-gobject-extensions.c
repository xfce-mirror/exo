/* $Id$ */
/*-
 * Copyright (c) 2004 os-cillation e.K.
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

#include <exo/exo-gobject-extensions.h>
#include <exo/exo-alias.h>



/**
 * exo_g_value_transform_negate:
 * @src_value : A value convertible to <type>gboolean</type>.
 * @dst_value : A value which can be assigned a <type>gboolean</type>.
 *
 * Applies boolean negation to @src_value and stores the result
 * in @dst_value.
 *
 * This function is mostly useful for binding boolean properties
 * with inversing.
 *
 * Return value: %TRUE on successful transformation.
 **/
gboolean
exo_g_value_transform_negate (const GValue  *src_value,
                              GValue        *dst_value)
{
  if (g_value_transform (src_value, dst_value))
    {
      g_value_set_boolean (dst_value, !g_value_get_boolean (dst_value));
      return TRUE;
    }

  return FALSE;
}



#define __EXO_GOBJECT_EXTENSIONS_C__
#include <exo/exo-aliasdef.c>
