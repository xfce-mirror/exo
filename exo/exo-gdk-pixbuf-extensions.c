/* $Id: exo-gdk-pixbuf-extensions.c,v 1.1.1.1 2004/09/14 22:32:58 bmeurer Exp $ */
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

#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <exo/exo-gdk-pixbuf-extensions.h>



/**
 * exo_gdk_pixbuf_scale_ratio:
 * @source      :
 * @dest_size   :
 *
 * Return value :
 **/
GdkPixbuf*
exo_gdk_pixbuf_scale_ratio (GdkPixbuf *source,
                            gint       dest_size)
{
  GdkPixbuf *dest;
  gdouble    wratio;
  gdouble    hratio;
  gint       source_width;
  gint       source_height;
  gint       dest_width;
  gint       dest_height;

  g_return_val_if_fail (GDK_IS_PIXBUF (source), NULL);
  g_return_val_if_fail (dest_size > 0, NULL);

  source_width  = gdk_pixbuf_get_width  (source);
  source_height = gdk_pixbuf_get_height (source);

  wratio = (gdouble) source_width  / (gdouble) dest_size;
  hratio = (gdouble) source_height / (gdouble) dest_size;

  if (hratio > wratio)
    {
      dest_width  = rint (source_width / hratio);
      dest_height = dest_size;
    }
  else
    {
      dest_width  = dest_size;
      dest_height = rint (source_height / wratio);
    }

  dest = gdk_pixbuf_scale_simple (source,
                                  dest_width,
                                  dest_height,
                                  GDK_INTERP_BILINEAR);

  return dest;
}




