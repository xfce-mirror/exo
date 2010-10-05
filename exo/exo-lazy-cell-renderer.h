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

#ifndef __EXO_LAZY_CELL_RENDERER_H__
#define __EXO_LAZY_CELL_RENDERER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS;

typedef struct _ExoLazyCellRendererIface ExoLazyCellRendererIface;
typedef struct _ExoLazyCellRenderer      ExoLazyCellRenderer;

#define EXO_TYPE_LAZY_CELL_RENDERER           (exo_lazy_cell_renderer_get_type ())
#define EXO_LAZY_CELL_RENDERER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_LAZY_CELL_RENDERER, ExoLazyCellRenderer))
#define EXO_IS_LAZY_CELL_RENDERER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_LAZY_CELL_RENDERER))
#define EXO_LAZY_CELL_RENDERER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), EXO_TYPE_LAZY_CELL_RENDERER, ExoLazyCellRendererIface))

struct _ExoLazyCellRendererIface
{
  /*< private >*/
  GTypeInterface __parent__;

  /*< public >*/
  gboolean (*render_and_resize) (ExoLazyCellRenderer *lazy_cell_renderer,
                                 GdkWindow           *window,
                                 GtkWidget           *widget,
                                 GdkRectangle        *background_area,
                                 GdkRectangle        *cell_area,
                                 GdkRectangle        *expose_area,
                                 GtkCellRendererState state,
                                 gint                *x_offset,
                                 gint                *y_offset,
                                 gint                *width,
                                 gint                *height);

  /*< private >*/
  void (*reserved1) (void);
  void (*reserved2) (void);
  void (*reserved3) (void);
};

GType     exo_lazy_cell_renderer_get_type          (void) G_GNUC_CONST;

gboolean  exo_lazy_cell_renderer_render_and_resize (ExoLazyCellRenderer *lazy_cell_renderer,
                                                    GdkWindow           *window,
                                                    GtkWidget           *widget,
                                                    GdkRectangle        *background_area,
                                                    GdkRectangle        *cell_area,
                                                    GdkRectangle        *expose_area,
                                                    GtkCellRendererState state,
                                                    gint                *x_offset,
                                                    gint                *y_offset,
                                                    gint                *width,
                                                    gint                *height);

G_END_DECLS;

#endif /* !__EXO_LAZY_CELL_RENDERER_H__ */
