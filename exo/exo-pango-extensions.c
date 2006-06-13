/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
 * Copyright (c) 2000      Anders Carlsson <andersca@gnu.org>
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

#include <exo/exo-pango-extensions.h>
#include <exo/exo-alias.h>



GType
exo_pango_ellipsize_mode_get_type (void)
{
  return pango_ellipsize_mode_get_type ();
}



/**
 * exo_pango_layout_set_text_ellipsized:
 * @layout : A #PangoLayout.
 * @string : The string to ellipsize.
 * @width  : The maximal width for the @layout.
 * @mode   : The ellipsize mode.
 * 
 * Truncates a string if required to fit in @width and sets it on the
 * layout. Truncation involves removing characters from the start, middle or end
 * respectively and replacing them with "...". Algorithm is a bit
 * fuzzy, won't work 100%.
 *
 * Return value: %TRUE if @string had to be ellipsized to fit into @width, else
 *               %FALSE.
 *
 * Deprecated: 0.3.1.8: Use pango_layout_set_ellipsize() instead.
 **/
gboolean
exo_pango_layout_set_text_ellipsized (PangoLayout       *layout,
                                      const gchar       *string,
                                      gint               width,
                                      PangoEllipsizeMode mode)
{
  g_return_val_if_fail (PANGO_IS_LAYOUT (layout), FALSE);
  g_return_val_if_fail (string != NULL, FALSE);
  g_return_val_if_fail (width >= 0, FALSE);

  pango_layout_set_text (layout, string, -1);
  pango_layout_set_width (layout, PANGO_SCALE * width);
  pango_layout_set_ellipsize (layout, mode);

  return (mode != EXO_PANGO_ELLIPSIZE_NONE);
}



#define __EXO_PANGO_EXTENSIONS_C__
#include <exo/exo-aliasdef.c>
