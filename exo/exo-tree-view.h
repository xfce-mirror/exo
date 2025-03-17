/*-
 * Copyright (c) 2004-2006 Benedikt Meurer <benny@xfce.org>
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

#ifndef __EXO_TREE_VIEW_H__
#define __EXO_TREE_VIEW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ExoTreeViewPrivate ExoTreeViewPrivate;
typedef struct _ExoTreeViewClass   ExoTreeViewClass;
typedef struct _ExoTreeView        ExoTreeView;

#define EXO_TYPE_TREE_VIEW            (exo_tree_view_get_type ())
#define EXO_TREE_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_TREE_VIEW, ExoTreeView))
#define EXO_TREE_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_TREE_VIEW, ExoTreeViewClass))
#define EXO_IS_TREE_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_TREE_VIEW))
#define EXO_IS_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_TREE_VIEW))
#define EXO_TREE_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_TREE_VIEW, ExoTreeViewClass))

struct _ExoTreeViewClass
{
  /*< private >*/
  GtkTreeViewClass __parent__;

  /*< private >*/
  void (*reserved1) (void);
  void (*reserved2) (void);
  void (*reserved3) (void);
  void (*reserved4) (void);
  void (*reserved5) (void);
  void (*reserved6) (void);
  void (*reserved7) (void);
  void (*reserved8) (void);
};

/**
 * ExoTreeView:
 *
 * The #ExoIconView struct contains only private fields and should
 * not be directly accessed.
 *
 * Deprecated: 4.21.0.
 **/
struct _ExoTreeView
{
  /*< private >*/
  GtkTreeView __parent__;

  /*< private >*/
  ExoTreeViewPrivate *priv;
};

GType      exo_tree_view_get_type                 (void) G_GNUC_CONST;

G_DEPRECATED_FOR (xfce_tree_view_new)
GtkWidget *exo_tree_view_new                      (void) G_GNUC_MALLOC;

G_DEPRECATED_FOR (xfce_tree_view_get_single_click)
gboolean   exo_tree_view_get_single_click         (const ExoTreeView *tree_view);
G_DEPRECATED_FOR (xfce_tree_view_set_single_click)
void       exo_tree_view_set_single_click         (ExoTreeView       *tree_view,
                                                   gboolean           single_click);

G_DEPRECATED_FOR (xfce_tree_view_get_single_click_timeout)
guint      exo_tree_view_get_single_click_timeout (const ExoTreeView *tree_view);
G_DEPRECATED_FOR (xfce_tree_view_set_single_click_timeout)
void       exo_tree_view_set_single_click_timeout (ExoTreeView       *tree_view,
                                                   guint              single_click_timeout);

G_END_DECLS

#endif /* !__EXO_TREE_VIEW_H__ */
