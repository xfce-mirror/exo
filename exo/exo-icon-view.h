/* $Id$ */
/*-
 * Copyright (c) 2004      os-cillation e.K.
 * Copyright (c) 2002,2004 Anders Carlsson <andersca@gnome.org>
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

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EXO_ICON_VIEW_H__
#define __EXO_ICON_VIEW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EXO_TYPE_ICON_VIEW             (exo_icon_view_get_type ())
#define EXO_ICON_VIEW(obj)		          (GTK_CHECK_CAST ((obj), EXO_TYPE_ICON_VIEW, ExoIconView))
#define EXO_ICON_VIEW_CLASS(klass)	    (GTK_CHECK_CLASS_CAST ((klass), EXO_TYPE_ICON_VIEW, ExoIconViewClass))
#define EXO_IS_ICON_VIEW(obj)		      (GTK_CHECK_TYPE ((obj), EXO_TYPE_ICON_VIEW))
#define EXO_IS_ICON_VIEW_CLASS(klass)	(GTK_CHECK_CLASS_TYPE ((klass), EXO_TYPE_ICON_VIEW))
#define EXO_ICON_VIEW_GET_CLASS(obj)   (GTK_CHECK_GET_CLASS ((obj), EXO_TYPE_ICON_VIEW, ExoIconViewClass))

typedef struct _ExoIconView           ExoIconView;
typedef struct _ExoIconViewClass      ExoIconViewClass;
typedef struct _ExoIconViewPrivate    ExoIconViewPrivate;

typedef void (* ExoIconViewForeachFunc) (ExoIconView     *icon_view,
					                                GtkTreePath      *path,
					                                gpointer          data);

struct _ExoIconView
{
  GtkContainer parent;

  /*< private >*/
  ExoIconViewPrivate *priv;
};

struct _ExoIconViewClass
{
  GtkContainerClass parent_class;

  void    (*set_scroll_adjustments) (ExoIconView     *icon_view,
				                             GtkAdjustment    *hadjustment,
				                             GtkAdjustment    *vadjustment);
  
  void    (*item_activated)         (ExoIconView     *icon_view,
				                             GtkTreePath      *path);
  void    (*selection_changed)      (ExoIconView     *icon_view);

  /* Key binding signals */
  void    (*select_all)             (ExoIconView     *icon_view);
  void    (*unselect_all)           (ExoIconView     *icon_view);
  void    (*select_cursor_item)     (ExoIconView     *icon_view);
  void    (*toggle_cursor_item)     (ExoIconView     *icon_view);
  gboolean (*move_cursor)           (ExoIconView     *icon_view,
				                             GtkMovementStep   step,
				                             gint              count);
  gboolean (*activate_cursor_item)  (ExoIconView     *icon_view);

  void  (*reserved1)  (void);
  void  (*reserved2)  (void);
  void  (*reserved3)  (void);
  void  (*reserved4)  (void);
};

GType      exo_icon_view_get_type       (void);
GtkWidget *exo_icon_view_new            (void);
GtkWidget *exo_icon_view_new_with_model (GtkTreeModel *model);

void          exo_icon_view_set_model         (ExoIconView *icon_view,
					                                      GtkTreeModel *model);
GtkTreeModel *exo_icon_view_get_model         (ExoIconView *icon_view);
void          exo_icon_view_set_text_column   (ExoIconView *icon_view,
                                                gint          column);
gint          exo_icon_view_get_text_column   (ExoIconView *icon_view);
void          exo_icon_view_set_markup_column (ExoIconView *icon_view,
                                                gint          column);
gint          exo_icon_view_get_markup_column (ExoIconView *icon_view);
void          exo_icon_view_set_pixbuf_column (ExoIconView *icon_view,
                                                gint          column);
gint          exo_icon_view_get_pixbuf_column (ExoIconView *icon_view);

void           exo_icon_view_set_orientation (ExoIconView   *icon_view,
                                               GtkOrientation  orientation);
GtkOrientation exo_icon_view_get_orientation (ExoIconView   *icon_view);


GtkTreePath *    exo_icon_view_get_path_at_pos    (ExoIconView           *icon_view,
                                                    gint                    x,
                                                    gint                    y);
void             exo_icon_view_selected_foreach   (ExoIconView           *icon_view,
                                                    ExoIconViewForeachFunc func,
                                                    gpointer                data);
void             exo_icon_view_set_selection_mode (ExoIconView           *icon_view,
                                                    GtkSelectionMode        mode);
GtkSelectionMode exo_icon_view_get_selection_mode (ExoIconView           *icon_view);
void             exo_icon_view_select_path        (ExoIconView           *icon_view,
                                                    GtkTreePath            *path);
void             exo_icon_view_unselect_path      (ExoIconView           *icon_view,
                                                    GtkTreePath            *path);
gboolean         exo_icon_view_path_is_selected   (ExoIconView           *icon_view,
                                                    GtkTreePath            *path);
GList           *exo_icon_view_get_selected_items (ExoIconView           *icon_view);
void             exo_icon_view_select_all         (ExoIconView           *icon_view);
void             exo_icon_view_unselect_all       (ExoIconView           *icon_view);
void             exo_icon_view_item_activated     (ExoIconView           *icon_view,
                                                    GtkTreePath            *path);
G_END_DECLS

#endif /* __EXO_ICON_VIEW_H__ */
