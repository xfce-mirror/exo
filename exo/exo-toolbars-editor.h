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

#ifndef __EXO_TOOLBARS_EDITOR_H__
#define __EXO_TOOLBARS_EDITOR_H__

#include <exo/exo-toolbars-view.h>

G_BEGIN_DECLS

#define EXO_TYPE_TOOLBARS_EDITOR            (exo_toolbars_editor_get_type ())
#define EXO_TOOLBARS_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_TOOLBARS_EDITOR, ExoToolbarsEditor))
#define EXO_TOOLBARS_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_TOOLBARS_EDITOR, ExoToolbarsEditorClass))
#define EXO_IS_TOOLBARS_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_TOOLBARS_EDITOR))
#define EXO_IS_TOOLBARS_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_TOOLBARS_EDITOR))
#define EXO_TOOLBARS_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_TOOLBARS_EDITOR, ExoToolbarsEditorClass))

typedef struct _ExoToolbarsEditorPrivate ExoToolbarsEditorPrivate;
typedef struct _ExoToolbarsEditorClass   ExoToolbarsEditorClass;
typedef struct _ExoToolbarsEditor        ExoToolbarsEditor;

struct _ExoToolbarsEditorClass
{
  GtkVBoxClass __parent__;

  void  (*reserved1)  (void);
  void  (*reserved2)  (void);
  void  (*reserved3)  (void);
  void  (*reserved4)  (void);
};

/**
 * ExoToolbarsEditor:
 *
 * The #ExoToolbarsEditor struct contains only private fields and
 * should not be directly accessed.
 **/
struct _ExoToolbarsEditor
{
  GtkVBox __parent__;

  /*< private >*/
  ExoToolbarsEditorPrivate *priv;
};


GType             exo_toolbars_editor_get_type        (void) G_GNUC_CONST;
GtkWidget        *exo_toolbars_editor_new             (GtkUIManager       *ui_manager);
GtkWidget        *exo_toolbars_editor_new_with_model  (GtkUIManager       *ui_manager,
                                                       ExoToolbarsModel   *model);

ExoToolbarsModel *exo_toolbars_editor_get_model       (ExoToolbarsEditor  *editor);
void              exo_toolbars_editor_set_model       (ExoToolbarsEditor  *editor,
                                                       ExoToolbarsModel   *model);

GtkUIManager     *exo_toolbars_editor_get_ui_manager  (ExoToolbarsEditor  *editor);
void              exo_toolbars_editor_set_ui_manager  (ExoToolbarsEditor  *editor,
                                                       GtkUIManager       *ui_manager);

G_END_DECLS

#endif /* !__EXO_TOOLBARS_EDITOR_H__ */
