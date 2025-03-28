/*-
 * Copyright (c) 2004-2006  os-cillation e.K.
 * Copyright (c) 2002,2004  Anders Carlsson <andersca@gnu.org>
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

#ifndef __EXO_ICON_VIEW_H__
#define __EXO_ICON_VIEW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ExoIconViewPrivate    ExoIconViewPrivate;
typedef struct _ExoIconViewClass      ExoIconViewClass;
typedef struct _ExoIconView           ExoIconView;

#define EXO_TYPE_ICON_VIEW            (exo_icon_view_get_type ())
#define EXO_ICON_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_ICON_VIEW, ExoIconView))
#define EXO_ICON_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_ICON_VIEW, ExoIconViewClass))
#define EXO_IS_ICON_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_ICON_VIEW))
#define EXO_IS_ICON_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_ICON_VIEW))
#define EXO_ICON_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_ICON_VIEW, ExoIconViewClass))

/**
 * ExoIconViewForeachFunc:
 * @icon_view : an #ExoIconView.
 * @path      : the current path.
 * @user_data : the user data supplied to exo_icon_view_selected_foreach().
 *
 * Callback function prototype, invoked for every selected path in the
 * @icon_view. See exo_icon_view_selected_foreach() for details.
 *
 * Deprecated: 4.21.0: Use XfceIconViewForeachFunc instead.
 **/
typedef void (*ExoIconViewForeachFunc) (ExoIconView *icon_view,
                                        GtkTreePath *path,
                                        gpointer     user_data);

/**
 * ExoIconViewSearchEqualFunc:
 * @model       : the #GtkTreeModel being searched.
 * @column      : the search column set by exo_icon_view_set_search_column().
 * @key         : the key string to compare with.
 * @iter        : the #GtkTreeIter of the current item.
 * @search_data : user data from exo_icon_view_set_search_equal_func().
 *
 * A function used for checking whether a row in @model matches a search key string
 * entered by the user. Note the return value is reversed from what you would normally
 * expect, though it has some similarity to strcmp() returning 0 for equal strings.
 *
 * Returns: %FALSE if the row matches, %TRUE otherwise.
 *
 * Deprecated: 4.21.0: Use XfceIconViewSearchEqualFunc instead.
 **/
typedef gboolean (*ExoIconViewSearchEqualFunc) (GtkTreeModel *model,
                                                gint          column,
                                                const gchar  *key,
                                                GtkTreeIter  *iter,
                                                gpointer      search_data);

/**
 * ExoIconViewSearchPositionFunc:
 * @icon_view     : an #ExoIconView.
 * @search_dialog : the search dialog window to place.
 * @user_data     : user data from exo_icon_view_set_search_position_func().
 *
 * A function used to place the @search_dialog for the @icon_view.
 *
 * Deprecated: 4.21.0: Use XfceIconViewSearchPositionFunc instead.
 **/
typedef void (*ExoIconViewSearchPositionFunc) (ExoIconView *icon_view,
                                               GtkWidget   *search_dialog,
                                               gpointer     user_data);

/**
 * ExoIconViewDropPosition:
 * @EXO_ICON_VIEW_NO_DROP    : no drop indicator.
 * @EXO_ICON_VIEW_DROP_INTO  : drop indicator on an item.
 * @EXO_ICON_VIEW_DROP_LEFT  : drop indicator on the left of an item.
 * @EXO_ICON_VIEW_DROP_RIGHT : drop indicator on the right of an item.
 * @EXO_ICON_VIEW_DROP_ABOVE : drop indicator above an item.
 * @EXO_ICON_VIEW_DROP_BELOW : drop indicator below an item.
 *
 * Specifies whether to display the drop indicator,
 * i.e. where to drop into the icon view.
 *
 * Deprecated: 4.21.0: Use XfceIconViewDropPosition instead.
 **/
typedef enum
{
  EXO_ICON_VIEW_NO_DROP,
  EXO_ICON_VIEW_DROP_INTO,
  EXO_ICON_VIEW_DROP_LEFT,
  EXO_ICON_VIEW_DROP_RIGHT,
  EXO_ICON_VIEW_DROP_ABOVE,
  EXO_ICON_VIEW_DROP_BELOW
} ExoIconViewDropPosition;

/**
 * ExoIconViewLayoutMode:
 * @EXO_ICON_VIEW_LAYOUT_ROWS : layout items in rows.
 * @EXO_ICON_VIEW_LAYOUT_COLS : layout items in columns.
 *
 * Specifies the layouting mode of an #ExoIconView. @EXO_ICON_VIEW_LAYOUT_ROWS
 * is the default, which lays out items vertically in rows from top to bottom.
 * @EXO_ICON_VIEW_LAYOUT_COLS lays out items horizontally in columns from left
 * to right.
 *
 * Deprecated: 4.21.0: Use XfceIconViewLayoutMode instead.
 **/
typedef enum
{
  EXO_ICON_VIEW_LAYOUT_ROWS,
  EXO_ICON_VIEW_LAYOUT_COLS
} ExoIconViewLayoutMode;

/**
 * ExoIconView:
 *
 * #ExoIconView provides an alternative view on a list model.
 * It displays the model as a grid of icons with labels. Like
 * #GtkTreeView, it allows to select one or multiple items
 * (depending on the selection mode, see exo_icon_view_set_selection_mode()).
 * In addition to selection with the arrow keys, #ExoIconView supports
 * rubberband selection, which is controlled by dragging the pointer.
 *
 * Deprecated: 4.21.0: Use XfceIconView instead.
 **/
struct _ExoIconView
{
  GtkContainer        __parent__;

  /*< private >*/
  ExoIconViewPrivate *priv;
};

struct _ExoIconViewClass
{
  GtkContainerClass __parent__;

  /* virtual methods */
  void     (*set_scroll_adjustments)    (ExoIconView     *icon_view,
                                         GtkAdjustment   *hadjustment,
                                         GtkAdjustment   *vadjustment);

  /* signals */
  void     (*item_activated)            (ExoIconView     *icon_view,
                                         GtkTreePath     *path);
  void     (*selection_changed)         (ExoIconView     *icon_view);

  /* Key binding signals */
  void     (*select_all)                (ExoIconView    *icon_view);
  void     (*unselect_all)              (ExoIconView    *icon_view);
  void     (*select_cursor_item)        (ExoIconView    *icon_view);
  void     (*toggle_cursor_item)        (ExoIconView    *icon_view);
  gboolean (*move_cursor)               (ExoIconView    *icon_view,
                                         GtkMovementStep step,
                                         gint            count);
  gboolean (*activate_cursor_item)      (ExoIconView    *icon_view);
  gboolean (*start_interactive_search)  (ExoIconView    *icon_view);

  /*< private >*/
  void (*reserved0) (void);
  void (*reserved1) (void);
  void (*reserved2) (void);
  void (*reserved3) (void);
  void (*reserved4) (void);
  void (*reserved5) (void);
  void (*reserved6) (void);
  void (*reserved7) (void);
  void (*reserved8) (void);
  void (*reserved9) (void);
};

GType                 exo_icon_view_get_type                  (void) G_GNUC_CONST;

G_DEPRECATED_FOR (xfce_icon_view_new)
GtkWidget            *exo_icon_view_new                       (void);
G_DEPRECATED_FOR (xfce_icon_view_new_with_model)
GtkWidget            *exo_icon_view_new_with_model            (GtkTreeModel             *model);

G_DEPRECATED_FOR (xfce_icon_view_get_model)
GtkTreeModel         *exo_icon_view_get_model                 (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_model)
void                  exo_icon_view_set_model                 (ExoIconView              *icon_view,
                                                               GtkTreeModel             *model);

G_DEPRECATED_FOR (xfce_icon_view_get_orientation)
GtkOrientation        exo_icon_view_get_orientation           (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_orientation)
void                  exo_icon_view_set_orientation           (ExoIconView              *icon_view,
                                                               GtkOrientation            orientation);

G_DEPRECATED_FOR (xfce_icon_view_get_columns)
gint                  exo_icon_view_get_columns               (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_columns)
void                  exo_icon_view_set_columns               (ExoIconView              *icon_view,
                                                               gint                      columns);

G_DEPRECATED_FOR (xfce_icon_view_get_item_width)
gint                  exo_icon_view_get_item_width            (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_item_width)
void                  exo_icon_view_set_item_width            (ExoIconView              *icon_view,
                                                               gint                      item_width);

G_DEPRECATED_FOR (xfce_icon_view_get_spacing)
gint                  exo_icon_view_get_spacing               (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_spacing)
void                  exo_icon_view_set_spacing               (ExoIconView              *icon_view,
                                                               gint                      spacing);

G_DEPRECATED_FOR (xfce_icon_view_get_row_spacing)
gint                  exo_icon_view_get_row_spacing           (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_row_spacing)
void                  exo_icon_view_set_row_spacing           (ExoIconView              *icon_view,
                                                               gint                      row_spacing);

G_DEPRECATED_FOR (xfce_icon_view_get_column_spacing)
gint                  exo_icon_view_get_column_spacing        (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_column_spacing)
void                  exo_icon_view_set_column_spacing        (ExoIconView              *icon_view,
                                                               gint                      column_spacing);

G_DEPRECATED_FOR (xfce_icon_view_get_margin)
gint                  exo_icon_view_get_margin                (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_margin)
void                  exo_icon_view_set_margin                (ExoIconView              *icon_view,
                                                               gint                      margin);

G_DEPRECATED_FOR (xfce_icon_view_get_selection_mode)
GtkSelectionMode      exo_icon_view_get_selection_mode        (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_selection_mode)
void                  exo_icon_view_set_selection_mode        (ExoIconView              *icon_view,
                                                               GtkSelectionMode          mode);

G_DEPRECATED_FOR (xfce_icon_view_get_layout_mode)
ExoIconViewLayoutMode exo_icon_view_get_layout_mode           (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_layout_mode)
void                  exo_icon_view_set_layout_mode           (ExoIconView              *icon_view,
                                                               ExoIconViewLayoutMode     layout_mode);

G_DEPRECATED_FOR (xfce_icon_view_get_single_click)
gboolean              exo_icon_view_get_single_click          (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_single_click)
void                  exo_icon_view_set_single_click          (ExoIconView              *icon_view,
                                                               gboolean                  single_click);

G_DEPRECATED_FOR (xfce_icon_view_get_single_click_timeout)
guint                 exo_icon_view_get_single_click_timeout  (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_single_click_timeout)
void                  exo_icon_view_set_single_click_timeout  (ExoIconView              *icon_view,
                                                               guint                     single_click_timeout);

G_DEPRECATED_FOR (xfce_icon_view_widget_to_icon_coords)
void                  exo_icon_view_widget_to_icon_coords     (const ExoIconView        *icon_view,
                                                               gint                      wx,
                                                               gint                      wy,
                                                               gint                     *ix,
                                                               gint                     *iy);
G_DEPRECATED_FOR (xfce_icon_view_icon_to_widget_coords)
void                  exo_icon_view_icon_to_widget_coords     (const ExoIconView        *icon_view,
                                                               gint                      ix,
                                                               gint                      iy,
                                                               gint                     *wx,
                                                               gint                     *wy);

G_DEPRECATED_FOR (xfce_icon_view_get_path_at_pos)
GtkTreePath          *exo_icon_view_get_path_at_pos           (const ExoIconView        *icon_view,
                                                               gint                      x,
                                                               gint                      y);
G_DEPRECATED_FOR (xfce_icon_view_get_item_at_pos)
gboolean              exo_icon_view_get_item_at_pos           (const ExoIconView        *icon_view,
                                                               gint                      x,
                                                               gint                      y,
                                                               GtkTreePath             **path,
                                                               GtkCellRenderer         **cell);

G_DEPRECATED_FOR (xfce_icon_view_get_visible_range)
gboolean              exo_icon_view_get_visible_range         (const ExoIconView        *icon_view,
                                                               GtkTreePath             **start_path,
                                                               GtkTreePath             **end_path);

G_DEPRECATED_FOR (xfce_icon_view_selected_foreach)
void                  exo_icon_view_selected_foreach          (ExoIconView              *icon_view,
                                                               ExoIconViewForeachFunc    func,
                                                               gpointer                  data);
G_DEPRECATED_FOR (xfce_icon_view_select_path)
void                  exo_icon_view_select_path               (ExoIconView              *icon_view,
                                                               GtkTreePath              *path);
G_DEPRECATED_FOR (xfce_icon_view_unselect_path)
void                  exo_icon_view_unselect_path             (ExoIconView              *icon_view,
                                                               GtkTreePath              *path);
G_DEPRECATED_FOR (xfce_icon_view_path_is_selected)
gboolean              exo_icon_view_path_is_selected          (const ExoIconView        *icon_view,
                                                               GtkTreePath              *path);
G_DEPRECATED_FOR (xfce_icon_view_get_selected_items)
GList                *exo_icon_view_get_selected_items        (const ExoIconView        *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_select_all)
void                  exo_icon_view_select_all                (ExoIconView              *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_unselect_all)
void                  exo_icon_view_unselect_all              (ExoIconView              *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_selection_invert)
void                  exo_icon_view_selection_invert          (ExoIconView              *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_item_activated)
void                  exo_icon_view_item_activated            (ExoIconView              *icon_view,
                                                               GtkTreePath              *path);

G_DEPRECATED_FOR (xfce_icon_view_get_item_column)
gint                  exo_icon_view_get_item_column           (ExoIconView              *icon_view,
                                                               GtkTreePath              *path);
G_DEPRECATED_FOR (xfce_icon_view_get_item_row)
gint                  exo_icon_view_get_item_row              (ExoIconView              *icon_view,
                                                               GtkTreePath              *path);

G_DEPRECATED_FOR (xfce_icon_view_get_cursor)
gboolean              exo_icon_view_get_cursor                (const ExoIconView        *icon_view,
                                                               GtkTreePath             **path,
                                                               GtkCellRenderer         **cell);
G_DEPRECATED_FOR (xfce_icon_view_set_cursor)
void                  exo_icon_view_set_cursor                (ExoIconView              *icon_view,
                                                               GtkTreePath              *path,
                                                               GtkCellRenderer          *cell,
                                                               gboolean                  start_editing);

G_DEPRECATED_FOR (xfce_icon_view_scroll_to_path)
void                  exo_icon_view_scroll_to_path            (ExoIconView              *icon_view,
                                                               GtkTreePath              *path,
                                                               gboolean                  use_align,
                                                               gfloat                    row_align,
                                                               gfloat                    col_align);

/* Drag-and-Drop support */
G_DEPRECATED_FOR (xfce_icon_view_enable_model_drag_source)
void                  exo_icon_view_enable_model_drag_source  (ExoIconView              *icon_view,
                                                               GdkModifierType           start_button_mask,
                                                               const GtkTargetEntry     *targets,
                                                               gint                      n_targets,
                                                               GdkDragAction             actions);
G_DEPRECATED_FOR (xfce_icon_view_enable_model_drag_dest)
void                  exo_icon_view_enable_model_drag_dest    (ExoIconView              *icon_view,
                                                               const GtkTargetEntry     *targets,
                                                               gint                      n_targets,
                                                               GdkDragAction             actions);
G_DEPRECATED_FOR (xfce_icon_view_unset_model_drag_source)
void                  exo_icon_view_unset_model_drag_source   (ExoIconView              *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_unset_model_drag_dest)
void                  exo_icon_view_unset_model_drag_dest     (ExoIconView              *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_reorderable)
void                  exo_icon_view_set_reorderable           (ExoIconView              *icon_view,
                                                               gboolean                  reorderable);
G_DEPRECATED_FOR (xfce_icon_view_get_reorderable)
gboolean              exo_icon_view_get_reorderable           (ExoIconView              *icon_view);


/* These are useful to implement your own custom stuff. */
G_DEPRECATED_FOR (xfce_icon_view_set_drag_dest_item)
void                  exo_icon_view_set_drag_dest_item        (ExoIconView              *icon_view,
                                                               GtkTreePath              *path,
                                                               ExoIconViewDropPosition   pos);
G_DEPRECATED_FOR (xfce_icon_view_get_drag_dest_item)
void                  exo_icon_view_get_drag_dest_item        (ExoIconView              *icon_view,
                                                               GtkTreePath             **path,
                                                               ExoIconViewDropPosition  *pos);
G_DEPRECATED_FOR (xfce_icon_view_get_dest_item_at_pos)
gboolean              exo_icon_view_get_dest_item_at_pos      (ExoIconView              *icon_view,
                                                               gint                      drag_x,
                                                               gint                      drag_y,
                                                               GtkTreePath             **path,
                                                               ExoIconViewDropPosition  *pos);
G_DEPRECATED_FOR (xfce_icon_view_create_drag_icon)
cairo_surface_t      *exo_icon_view_create_drag_icon          (ExoIconView              *icon_view,
                                                               GtkTreePath              *path);


/* Interactive search support */
G_DEPRECATED_FOR (xfce_icon_view_get_enable_search)
gboolean                      exo_icon_view_get_enable_search         (const ExoIconView            *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_enable_search)
void                          exo_icon_view_set_enable_search         (ExoIconView                  *icon_view,
                                                                       gboolean                      enable_search);
G_DEPRECATED_FOR (xfce_icon_view_get_search_column)
gint                          exo_icon_view_get_search_column         (const ExoIconView            *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_search_column)
void                          exo_icon_view_set_search_column         (ExoIconView                  *icon_view,
                                                                       gint                          search_column);
G_DEPRECATED_FOR (xfce_icon_view_get_search_equal_func)
ExoIconViewSearchEqualFunc    exo_icon_view_get_search_equal_func     (const ExoIconView            *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_search_equal_func)
void                          exo_icon_view_set_search_equal_func     (ExoIconView                  *icon_view,
                                                                       ExoIconViewSearchEqualFunc    search_equal_func,
                                                                       gpointer                      search_equal_data,
                                                                       GDestroyNotify                search_equal_destroy);
G_DEPRECATED_FOR (xfce_icon_view_get_search_position_func)
ExoIconViewSearchPositionFunc exo_icon_view_get_search_position_func  (const ExoIconView            *icon_view);
G_DEPRECATED_FOR (xfce_icon_view_set_search_position_func)
void                          exo_icon_view_set_search_position_func  (ExoIconView                  *icon_view,
                                                                       ExoIconViewSearchPositionFunc search_position_func,
                                                                       gpointer                      search_position_data,
                                                                       GDestroyNotify                search_position_destroy);

G_END_DECLS

#endif /* __EXO_ICON_VIEW_H__ */
