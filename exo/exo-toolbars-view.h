/*-
 * Copyright (c) 2004 os-cillation e.K.
 * Copyright (c) 2003 Marco Pesenti Gritti
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

#ifndef __EXO_TOOLBARS_VIEW_H__
#define __EXO_TOOLBARS_VIEW_H__

#include <exo/exo-toolbars-model.h>

G_BEGIN_DECLS

#define EXO_TYPE_TOOLBARS_VIEW            (exo_toolbars_view_get_type ())
#define EXO_TOOLBARS_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_TOOLBARS_VIEW, ExoToolbarsView))
#define EXO_TOOLBARS_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_TOOLBARS_VIEW, ExoToolbarsViewClass))
#define EXO_IS_TOOLBARS_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_TOOLBARS_VIEW))
#define EXO_IS_TOOLBARS_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_TOOLBARS_VIEW))
#define EXO_TOOLBARS_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_TOOLBARS_VIEW, ExoToolbarsViewClass))

typedef struct _ExoToolbarsViewPrivate ExoToolbarsViewPrivate;
typedef struct _ExoToolbarsViewClass   ExoToolbarsViewClass;
typedef struct _ExoToolbarsView        ExoToolbarsView;

struct _ExoToolbarsViewClass
{
  GtkVBoxClass __parent__;

  /* signals */
  void  (*action_request) (ExoToolbarsView  *view,
                           const gchar      *action_name);
  void  (*customize)      (ExoToolbarsView  *view);

  void  (*reserved1)      (void);
  void  (*reserved2)      (void);
  void  (*reserved3)      (void);
  void  (*reserved4)      (void);
};

/**
 * ExoToolbarsView:
 *
 * The #ExoToolbarsView struct contains only private fields and should
 * not be directly accessed.
 **/
struct _ExoToolbarsView
{
  GtkVBox __parent__;

  /*< private >*/
  ExoToolbarsViewPrivate *priv;
};


GType             exo_toolbars_view_get_type        (void) G_GNUC_CONST;
GtkWidget        *exo_toolbars_view_new             (GtkUIManager         *ui_manager);
GtkWidget        *exo_toolbars_view_new_with_model  (GtkUIManager         *ui_manager,
                                                     ExoToolbarsModel     *model);

gboolean          exo_toolbars_view_get_editing     (ExoToolbarsView      *view);
void              exo_toolbars_view_set_editing     (ExoToolbarsView      *view,
                                                     gboolean              editing);

ExoToolbarsModel *exo_toolbars_view_get_model       (ExoToolbarsView      *view);
void              exo_toolbars_view_set_model       (ExoToolbarsView      *view,
                                                     ExoToolbarsModel     *model);

GtkUIManager     *exo_toolbars_view_get_ui_manager  (ExoToolbarsView      *view);
void              exo_toolbars_view_set_ui_manager  (ExoToolbarsView      *view,
                                                     GtkUIManager         *ui_manager);

G_END_DECLS

#endif /* !__EXO_TOOLBARS_VIEW_H__ */
