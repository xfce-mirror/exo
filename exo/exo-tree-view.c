/* $Id$ */
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo/exo-config.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-tree-view.h>
#include <exo/exo-alias.h>



/* the hover auto-select delay (in ms) */
#define EXO_TREE_VIEW_HOVER_TIMEOUT (500)



#define EXO_TREE_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_TREE_VIEW, ExoTreeViewPrivate))



/* Property identifiers */
enum
{
  PROP_0,
  PROP_SINGLE_CLICK,
};



static void     exo_tree_view_class_init            (ExoTreeViewClass *klass);
static void     exo_tree_view_init                  (ExoTreeView      *tree_view);
static void     exo_tree_view_finalize              (GObject          *object);
static void     exo_tree_view_get_property          (GObject          *object,
                                                     guint             prop_id,
                                                     GValue           *value,
                                                     GParamSpec       *pspec);
static void     exo_tree_view_set_property          (GObject          *object,
                                                     guint             prop_id,
                                                     const GValue     *value,
                                                     GParamSpec       *pspec);
static gboolean exo_tree_view_button_press_event    (GtkWidget        *widget,
                                                     GdkEventButton   *event);
static gboolean exo_tree_view_button_release_event  (GtkWidget        *widget,
                                                     GdkEventButton   *event);
static gboolean exo_tree_view_motion_notify_event   (GtkWidget        *widget,
                                                     GdkEventMotion   *event);
static gboolean exo_tree_view_leave_notify_event    (GtkWidget        *widget,
                                                     GdkEventCrossing *event);
static void     exo_tree_view_drag_begin            (GtkWidget        *widget,
                                                     GdkDragContext   *context);



struct _ExoTreeViewPrivate
{
  /* single click mode */
  guint        single_click : 1;

  /* whether the next button-release-event should emit "row-activate" */
  guint        button_release_activates : 1;

  /* the path below the pointer or NULL */
  GtkTreePath *hover_path;
};



static GObjectClass *exo_tree_view_parent_class;



GType
exo_tree_view_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ExoTreeViewClass),
        NULL,
        NULL,
        (GClassInitFunc) exo_tree_view_class_init,
        NULL,
        NULL,
        sizeof (ExoTreeView),
        0,
        (GInstanceInitFunc) exo_tree_view_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_TREE_VIEW, I_("ExoTreeView"), &info, 0);
    }

  return type;
}



static void
exo_tree_view_class_init (ExoTreeViewClass *klass)
{
  GtkWidgetClass *gtkwidget_class;
  GObjectClass   *gobject_class;

  /* add our private data to the class */
  g_type_class_add_private (klass, sizeof (ExoTreeViewPrivate));

  /* determine our parent type class */
  exo_tree_view_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_tree_view_finalize;
  gobject_class->get_property = exo_tree_view_get_property;
  gobject_class->set_property = exo_tree_view_set_property;

  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->button_press_event = exo_tree_view_button_press_event;
  gtkwidget_class->button_release_event = exo_tree_view_button_release_event;
  gtkwidget_class->motion_notify_event = exo_tree_view_motion_notify_event;
  gtkwidget_class->leave_notify_event = exo_tree_view_leave_notify_event;
  gtkwidget_class->drag_begin = exo_tree_view_drag_begin;

  /* initialize the library's i18n support */
  _exo_i18n_init ();

  /**
   * ExoIconView:single-click:
   *
   * %TRUE to activate items using a single click instead of a
   * double click.
   *
   * Since: 0.3.1.3
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_SINGLE_CLICK,
                                   g_param_spec_boolean ("single-click",
                                                         _("Single Click"),
                                                         _("Whether the items in the view can be activated with single clicks"),
                                                         FALSE,
                                                         EXO_PARAM_READWRITE));
}



static void
exo_tree_view_init (ExoTreeView *tree_view)
{
  /* grab a pointer on the private data */
  tree_view->priv = EXO_TREE_VIEW_GET_PRIVATE (tree_view);
}



static void
exo_tree_view_finalize (GObject *object)
{
  ExoTreeView *tree_view = EXO_TREE_VIEW (object);

  /* be sure to release the hover path */
  if (G_UNLIKELY (tree_view->priv->hover_path == NULL))
    gtk_tree_path_free (tree_view->priv->hover_path);

  (*G_OBJECT_CLASS (exo_tree_view_parent_class)->finalize) (object);
}



static void
exo_tree_view_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  ExoTreeView *tree_view = EXO_TREE_VIEW (object);

  switch (prop_id)
    {
    case PROP_SINGLE_CLICK:
      g_value_set_boolean (value, exo_tree_view_get_single_click (tree_view));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_tree_view_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  ExoTreeView *tree_view = EXO_TREE_VIEW (object);

  switch (prop_id)
    {
    case PROP_SINGLE_CLICK:
      exo_tree_view_set_single_click (tree_view, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gboolean
exo_tree_view_button_press_event (GtkWidget      *widget,
                                  GdkEventButton *event)
{
  GtkTreeSelection *selection;
  ExoTreeView      *tree_view = EXO_TREE_VIEW (widget);
  GtkTreePath      *path;
  gboolean          result;
  GList            *selected_paths = NULL;
  GList            *lp;

  /* grab the tree selection */
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));

  /* determine the path at the event coordinates */
  if (!gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (tree_view), event->x, event->y, &path, NULL, NULL, NULL))
    path = NULL;

  /* we unselect all selected items if the user clicks on an empty
   * area of the tree view and no modifier key is active.
   */
  if (path == NULL && (event->state & gtk_accelerator_get_default_mod_mask ()) == 0)
    gtk_tree_selection_unselect_all (selection);

  /* completely ignore double-clicks in single-click mode */
  if (tree_view->priv->single_click && event->type == GDK_2BUTTON_PRESS)
    {
      /* make sure we ignore the GDK_BUTTON_RELEASE
       * event for this GDK_2BUTTON_PRESS event.
       */
      tree_view->priv->button_release_activates = FALSE;
      return TRUE;
    }

  /* check if the next button-release-event should activate the selected row (single click support) */
  tree_view->priv->button_release_activates = (tree_view->priv->single_click && event->type == GDK_BUTTON_PRESS && event->button == 1
                                               && (event->state & gtk_accelerator_get_default_mod_mask ()) == 0);

  /* unfortunately GtkTreeView will unselect rows except the clicked one,
   * which makes dragging from a GtkTreeView problematic. That's why we
   * remember the selected paths here and restore them later.
   */
  if (path != NULL && gtk_tree_selection_path_is_selected (selection, path))
    selected_paths = gtk_tree_selection_get_selected_rows (selection, NULL);

  /* call the parent's button press handler */
  result = (*GTK_WIDGET_CLASS (exo_tree_view_parent_class)->button_press_event) (widget, event);

  /* restore previous selection if the path is still selected */
  if (path != NULL && gtk_tree_selection_path_is_selected (selection, path))
    {
      /* select all previously selected paths */
      for (lp = selected_paths; lp != NULL; lp = lp->next)
        gtk_tree_selection_select_path (selection, lp->data);
    }

  /* release the path (if any) */
  if (G_LIKELY (path != NULL))
    gtk_tree_path_free (path);

  /* release the selected paths list */
  g_list_foreach (selected_paths, (GFunc) gtk_tree_path_free, NULL);
  g_list_free (selected_paths);

  return result;
}



static gboolean
exo_tree_view_button_release_event (GtkWidget      *widget,
                                    GdkEventButton *event)
{
  GtkTreeViewColumn *column;
  GtkTreePath       *path;
  ExoTreeView       *tree_view = EXO_TREE_VIEW (widget);

  /* check if we're in single-click mode and the button-release-event should emit a "row-activate" */
  if (G_UNLIKELY (tree_view->priv->single_click && tree_view->priv->button_release_activates))
    {
      /* reset the "release-activates" flag */
      tree_view->priv->button_release_activates = FALSE;

      /* determine the path to the row that should be activated */
      if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (tree_view), event->x, event->y, &path, &column, NULL, NULL))
        {
          /* emit row-activated for the determined row */
          gtk_tree_view_row_activated (GTK_TREE_VIEW (tree_view), path, column);

          /* cleanup */
          gtk_tree_path_free (path);
        }
    }

  /* call the parent's button release handler */
  return (*GTK_WIDGET_CLASS (exo_tree_view_parent_class)->button_release_event) (widget, event);
}



static gboolean
exo_tree_view_motion_notify_event (GtkWidget      *widget,
                                   GdkEventMotion *event)
{
  ExoTreeView *tree_view = EXO_TREE_VIEW (widget);
  GtkTreePath *path;
  GdkCursor   *cursor;

  /* check if the event occurred on the tree view internal window and we are in single-click mode */
  if (event->window == gtk_tree_view_get_bin_window (GTK_TREE_VIEW (tree_view)) && tree_view->priv->single_click)
    {
      /* determine the path at the event coordinates */
      if (!gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (tree_view), event->x, event->y, &path, NULL, NULL, NULL))
        path = NULL;

      /* check if we have a new path */
      if ((path == NULL && tree_view->priv->hover_path != NULL) || (path != NULL && tree_view->priv->hover_path == NULL)
          || (path != NULL && tree_view->priv->hover_path != NULL && gtk_tree_path_compare (path, tree_view->priv->hover_path) != 0))
        {
          /* release the previous hover path */
          if (tree_view->priv->hover_path != NULL)
            gtk_tree_path_free (tree_view->priv->hover_path);

          /* setup the new path */
          tree_view->priv->hover_path = path;

          /* check if we're over a row right now */
          if (G_LIKELY (path != NULL))
            {
              /* setup the hand cursor to indicate that the row at the pointer can be activated with a single click */
              cursor = gdk_cursor_new (GDK_HAND2);
              gdk_window_set_cursor (event->window, cursor);
              gdk_cursor_unref (cursor);
            }
          else
            {
              /* reset the cursor to its default */
              gdk_window_set_cursor (event->window, NULL);
            }
        }
      else
        {
          /* release the path resources */
          if (path != NULL)
            gtk_tree_path_free (path);
        }
    }

  /* call the parent's motion notify handler */
  return (*GTK_WIDGET_CLASS (exo_tree_view_parent_class)->motion_notify_event) (widget, event);
}



static gboolean
exo_tree_view_leave_notify_event (GtkWidget        *widget,
                                  GdkEventCrossing *event)
{
  ExoTreeView *tree_view = EXO_TREE_VIEW (widget);

  /* release and reset the hover path (if any) */
  if (tree_view->priv->hover_path != NULL)
    {
      gtk_tree_path_free (tree_view->priv->hover_path);
      tree_view->priv->hover_path = NULL;
    }

  /* reset the cursor for the tree view internal window */
  if (GTK_WIDGET_REALIZED (tree_view))
    gdk_window_set_cursor (gtk_tree_view_get_bin_window (GTK_TREE_VIEW (tree_view)), NULL);

  /* the next button-release-event should not activate */
  tree_view->priv->button_release_activates = FALSE;

  /* call the parent's leave notify handler */
  return (*GTK_WIDGET_CLASS (exo_tree_view_parent_class)->leave_notify_event) (widget, event);
}



static void
exo_tree_view_drag_begin (GtkWidget      *widget,
                          GdkDragContext *context)
{
  ExoTreeView *tree_view = EXO_TREE_VIEW (widget);

  /* the next button-release-event should not activate */
  tree_view->priv->button_release_activates = FALSE;

  /* call the parent's drag begin handler */
  return (*GTK_WIDGET_CLASS (exo_tree_view_parent_class)->drag_begin) (widget, context);
}



/**
 * exo_tree_view_new:
 *
 * Allocates a new #ExoTreeView instance.
 *
 * Return value: the newly allocated #ExoTreeView.
 *
 * Since: 0.3.1.3
 **/
GtkWidget*
exo_tree_view_new (void)
{
  return g_object_new (EXO_TYPE_TREE_VIEW, NULL);
}



/**
 * exo_tree_view_get_single_click:
 * @tree_view : an #ExoTreeView.
 *
 * Returns %TRUE if @tree_view is in single-click mode, else %FALSE.
 *
 * Return value: whether @tree_view is in single-click mode.
 *
 * Since: 0.3.1.3
 **/
gboolean
exo_tree_view_get_single_click (const ExoTreeView *tree_view)
{
  g_return_val_if_fail (EXO_IS_TREE_VIEW (tree_view), FALSE);
  return tree_view->priv->single_click;
}



/**
 * exo_tree_view_set_single_click:
 * @tree_view    : an #ExoTreeView.
 * @single_click : %TRUE to use single-click for @tree_view, %FALSE otherwise.
 *
 * If @single_click is %TRUE, @tree_view will use single-click mode, else
 * the default double-click mode will be used.
 *
 * Since: 0.3.1.3
 **/
void
exo_tree_view_set_single_click (ExoTreeView *tree_view,
                                gboolean     single_click)
{
  g_return_if_fail (EXO_IS_TREE_VIEW (tree_view));

  if (tree_view->priv->single_click != !!single_click)
    {
      tree_view->priv->single_click = !!single_click;
      g_object_notify (G_OBJECT (tree_view), "single-click");
    }
}



#define __EXO_TREE_VIEW_C__
#include <exo/exo-aliasdef.c>
