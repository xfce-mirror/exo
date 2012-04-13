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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EXO_ICON_BAR_H__
#define __EXO_ICON_BAR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EXO_TYPE_ICON_BAR             (exo_icon_bar_get_type ())
#define EXO_ICON_BAR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_ICON_BAR, ExoIconBar))
#define EXO_ICON_BAR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_ICON_BAR, ExoIconBarClass))
#define EXO_IS_ICON_BAR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_ICON_BAR))
#define EXO_IS_ICON_BAR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((obj), EXO_TYPE_ICON_BAR))
#define EXO_ICON_BAR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_ICON_BAR, ExoIconBarClass))

typedef struct _ExoIconBarPrivate ExoIconBarPrivate;
typedef struct _ExoIconBarClass   ExoIconBarClass;
typedef struct _ExoIconBar        ExoIconBar;

/**
 * ExoIconBarClass:
 * @set_scroll_adjustments : Used internally to make the ExoIconBar scrollable.
 * @selection_changed      : This signal is emitted whenever the currently selected icon changes.
 **/
struct _ExoIconBarClass
{
  GtkContainerClass __parent__;

  /* signals */
  void  (*set_scroll_adjustments)  (ExoIconBar    *icon_bar,
                                    GtkAdjustment *hadjustment,
                                    GtkAdjustment *vadjustment);
  void  (*selection_changed)       (ExoIconBar *icon_bar);

  /*< private >*/
  void  (*reserved1) (void);
  void  (*reserved2) (void);
  void  (*reserved3) (void);
  void  (*reserved4) (void);
};

/**
 * ExoIconBar:
 *
 * The #ExoIconBar struct contains only private fields and should not
 * be directly accessed.
 **/
struct _ExoIconBar
{
  GtkContainer       __parent__;

  /*< private >*/
  ExoIconBarPrivate *priv;
};

GType           exo_icon_bar_get_type           (void) G_GNUC_CONST;

GtkWidget      *exo_icon_bar_new                (void);
GtkWidget      *exo_icon_bar_new_with_model     (GtkTreeModel   *model);

GtkTreeModel   *exo_icon_bar_get_model          (ExoIconBar     *icon_bar);
void            exo_icon_bar_set_model          (ExoIconBar     *icon_bar,
                                                 GtkTreeModel   *model);

gint            exo_icon_bar_get_pixbuf_column  (ExoIconBar     *icon_bar);
void            exo_icon_bar_set_pixbuf_column  (ExoIconBar     *icon_bar,
                                                 gint            column);

gint            exo_icon_bar_get_text_column    (ExoIconBar     *icon_bar);
void            exo_icon_bar_set_text_column    (ExoIconBar     *icon_bar,
                                                 gint            column);

GtkOrientation  exo_icon_bar_get_orientation    (ExoIconBar     *icon_bar);
void            exo_icon_bar_set_orientation    (ExoIconBar     *icon_bar,
                                                 GtkOrientation  orientation);

gint            exo_icon_bar_get_active         (ExoIconBar     *icon_bar);
void            exo_icon_bar_set_active         (ExoIconBar     *icon_bar,
                                                 gint            idx);

gboolean        exo_icon_bar_get_active_iter    (ExoIconBar     *icon_bar,
                                                 GtkTreeIter    *iter);
void            exo_icon_bar_set_active_iter    (ExoIconBar     *icon_bar,
                                                 GtkTreeIter    *iter);

G_END_DECLS

#endif /* !__EXO_ICON_BAR_H__ */

