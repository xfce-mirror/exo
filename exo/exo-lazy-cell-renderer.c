/* $Id$ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>.
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

#include <exo/exo-lazy-cell-renderer.h>
#include <exo/exo-string.h>
#include <exo/exo-alias.h>



GType
exo_lazy_cell_renderer_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ExoLazyCellRendererIface),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        0,
        NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, I_("ExoLazyCellRenderer"), &info, 0);
      g_type_interface_add_prerequisite (type, GTK_TYPE_CELL_RENDERER);
    }

  return type;
}



/**
 * exo_lazy_cell_renderer_render_and_resize:
 * @lazy_cell_renderer : a #ExoLazyCellRenderer instance.
 * @window             : a #GdkDrawable to draw to.
 * @widget             : the widget owning @window.
 * @background_area    : entire cell area (including tree expanders and maybe padding on the sides).
 * @cell_area          : area normally rendered by a cell renderer.
 * @expose_area        : area that actually needs updating.
 * @flags              : flags that affect rendering.
 * @x_offset           : location to return x offset of cell relative to @cell_area or %NULL.
 * @y_offset           : location to return y offset of cell relative to @cell_area or %NULL.
 * @width              : location to return width needed to render a cell, or %NULL.
 * @height             : location to return height needed to render a cell, or %NULL.
 *
 * This method is an advanced version of gtk_cell_renderer_render() to support cell
 * renderers that lazily determine the really required size for an item cell, e.g.
 * an icon renderer for the #ExoIconView, which loads icons only on demand (when
 * needed for drawing).
 *
 * The implementation of this method must return the actual size required for the
 * cell in @width and @height and the offset relative to @cell_area in @x_offset
 * and @y_offset. The @width and @height must be smaller than the @cell_area
 * specified by the caller.
 *
 * Return value: %TRUE if the @lazy_cell_renderer wants the view to change the
 *               cell dimensions to whatever it placed in @x_offset,
 *               @y_offset, @width and @height, else %FALSE.
 **/
gboolean
exo_lazy_cell_renderer_render_and_resize (ExoLazyCellRenderer *lazy_cell_renderer,
                                          GdkWindow           *window,
                                          GtkWidget           *widget,
                                          GdkRectangle        *background_area,
                                          GdkRectangle        *cell_area,
                                          GdkRectangle        *expose_area,
                                          GtkCellRendererState state,
                                          gint                *x_offset,
                                          gint                *y_offset,
                                          gint                *width,
                                          gint                *height)
{
  g_return_val_if_fail (EXO_IS_LAZY_CELL_RENDERER (lazy_cell_renderer), FALSE);
  g_return_val_if_fail (GDK_IS_WINDOW (window), FALSE);
  g_return_val_if_fail (GTK_IS_WIDGET (widget), FALSE);

  return (*EXO_LAZY_CELL_RENDERER_GET_IFACE (lazy_cell_renderer)->render_and_resize) (lazy_cell_renderer,
                                                                                      window,
                                                                                      widget,
                                                                                      background_area,
                                                                                      cell_area,
                                                                                      expose_area,
                                                                                      state,
                                                                                      x_offset,
                                                                                      y_offset,
                                                                                      width,
                                                                                      height);
}



#define __EXO_LAZY_CELL_RENDERER_C__
#include <exo/exo-aliasdef.c>
