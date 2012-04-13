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

#ifndef __EXO_TOOLBARS_MODEL_H__
#define __EXO_TOOLBARS_MODEL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EXO_TYPE_TOOLBARS_MODEL             (exo_toolbars_model_get_type ())
#define EXO_TOOLBARS_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_TOOLBARS_MODEL, ExoToolbarsModel))
#define EXO_TOOLBARS_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_TOOLBARS_MODEL, ExoToolbarsModelClass))
#define EXO_IS_TOOLBARS_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_TOOLBARS_MODEL))
#define EXO_IS_TOOLBARS_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_TOOLBARS_MODEL))
#define EXO_TOOLBARS_MODEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_TOOLBARS_MODEL, ExoToolbarsModelClass))

typedef struct _ExoToolbarsModelPrivate ExoToolbarsModelPrivate;
typedef struct _ExoToolbarsModelClass   ExoToolbarsModelClass;
typedef struct _ExoToolbarsModel        ExoToolbarsModel;

/**
 * ExoToolbarsModelFlags:
 * @EXO_TOOLBARS_MODEL_NOT_REMOVABLE     : Not possible to remove items from the model.
 * @EXO_TOOLBARS_MODEL_ACCEPT_ITEMS_ONLY : Only accept new item, dragging items around
 *                                         is disabled.
 * @EXO_TOOLBARS_MODEL_OVERRIDE_STYLE    : If set, the #ExoToolbarsModel
 *                                         accepts a custom #GtkToolbarStyle. See
 *                                         exo_toolbars_model_set_style().
 */
typedef enum
{
  EXO_TOOLBARS_MODEL_NOT_REMOVABLE      = 1 << 0,
  EXO_TOOLBARS_MODEL_ACCEPT_ITEMS_ONLY  = 1 << 1,
  EXO_TOOLBARS_MODEL_OVERRIDE_STYLE     = 1 << 2,
} ExoToolbarsModelFlags;


/**
 * EXO_TOOLBARS_ITEM_TYPE:
 *
 * The string used for drag-and-drop in the toolbars editor/model.
 **/
#define EXO_TOOLBARS_ITEM_TYPE "application/x-exo-toolbars-item"

struct _ExoToolbarsModelClass
{
  GObjectClass  __parent__;

  /* Virtual Table */
  gboolean     (*add_item)  (ExoToolbarsModel *model,
                             gint              toolbar_position,
                             gint              item_position,
                             const gchar      *id,
                             const gchar      *type);

  void   (*reserved1)  (void);
  void   (*reserved2)  (void);
  void   (*reserved3)  (void);
  void   (*reserved4)  (void);

  /* Signals */
  void   (*item_added)        (ExoToolbarsModel *model,
                               gint              toolbar_position,
                               gint              item_position);
  void   (*item_removed)      (ExoToolbarsModel *model,
                               gint              toolbar_position,
                               gint              item_position);
  void   (*toolbar_added)     (ExoToolbarsModel *model,
                               gint              toolbar_position);
  void   (*toolbar_changed)   (ExoToolbarsModel *model,
                               gint              toolbar_position);
  void   (*toolbar_removed)   (ExoToolbarsModel *model,
                               gint              toolbar_position);
  gchar *(*get_item_type)     (ExoToolbarsModel *model,
                               GdkAtom           dnd_type);
  gchar *(*get_item_id)       (ExoToolbarsModel *model,
                               const gchar      *type,
                               const gchar      *data);
  gchar *(*get_item_data)     (ExoToolbarsModel *model,
                               const gchar      *type,
                               const gchar      *id);

  void   (*reserved5)  (void);
  void   (*reserved6)  (void);
  void   (*reserved7)  (void);
  void   (*reserved8)  (void);
};

/**
 * ExoToolbarsModel:
 *
 * The #ExoToolbarsModel struct contains only private fields and should
 * not be directly accessed.
 **/
struct _ExoToolbarsModel
{
  GObject                  __parent__;

  /*< private >*/
  ExoToolbarsModelPrivate *priv;
};


GType                 exo_toolbars_model_get_type       (void) G_GNUC_CONST;
ExoToolbarsModel     *exo_toolbars_model_new            (void);

void                  exo_toolbars_model_set_actions    (ExoToolbarsModel      *model,
                                                         gchar                **actions,
                                                         guint                  n_actions);
gchar               **exo_toolbars_model_get_actions    (ExoToolbarsModel      *model);

gboolean              exo_toolbars_model_load_from_file (ExoToolbarsModel      *model,
                                                         const gchar           *filename,
                                                         GError               **error);
gboolean              exo_toolbars_model_save_to_file   (ExoToolbarsModel      *model,
                                                         const gchar           *filename,
                                                         GError               **error);

ExoToolbarsModelFlags exo_toolbars_model_get_flags      (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position);
void                  exo_toolbars_model_set_flags      (ExoToolbarsModel      *model,
                                                         ExoToolbarsModelFlags  flags,
                                                         gint                   toolbar_position);

GtkToolbarStyle       exo_toolbars_model_get_style      (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position);
void                  exo_toolbars_model_set_style      (ExoToolbarsModel      *model,
                                                         GtkToolbarStyle        style,
                                                         gint                   toolbar_position);
void                  exo_toolbars_model_unset_style    (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position);

gchar                *exo_toolbars_model_get_item_type  (ExoToolbarsModel      *model,
                                                         GdkAtom                dnd_type);
gchar                *exo_toolbars_model_get_item_id    (ExoToolbarsModel      *model,
                                                         const gchar           *type,
                                                         const gchar           *name);
gchar                *exo_toolbars_model_get_item_data  (ExoToolbarsModel      *model,
                                                         const gchar           *type,
                                                         const gchar           *id);

gboolean              exo_toolbars_model_add_item       (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position,
                                                         gint                   item_position,
                                                         const gchar           *id,
                                                         const gchar           *type);
void                  exo_toolbars_model_add_separator  (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position,
                                                         gint                   item_position);
gint                  exo_toolbars_model_add_toolbar    (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position,
                                                         const gchar           *name);

void                  exo_toolbars_model_move_item      (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position,
                                                         gint                   item_position,
                                                         gint                   new_toolbar_position,
                                                         gint                   new_item_position);

void                  exo_toolbars_model_remove_item    (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position,
                                                         gint                   item_position);
void                  exo_toolbars_model_remove_toolbar (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position);

gint                  exo_toolbars_model_n_items        (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position);
void                  exo_toolbars_model_item_nth       (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position,
                                                         gint                   item_position,
                                                         gboolean              *is_separator,
                                                         const gchar          **id,
                                                         const gchar          **type);

gint                  exo_toolbars_model_n_toolbars     (ExoToolbarsModel      *model);
const gchar          *exo_toolbars_model_toolbar_nth    (ExoToolbarsModel      *model,
                                                         gint                   toolbar_position);

G_END_DECLS

#endif /* !__EXO_TOOLBARS_MODEL_H__ */
