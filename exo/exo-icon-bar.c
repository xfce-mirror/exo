/* $Id$ */
/*-
 * Copyright (c) 2004-2005 os-cillation e.K.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxfce4util/libxfce4util.h>

#include <exo/exo-config.h>
#include <exo/exo-icon-bar.h>
#include <exo/exo-marshal.h>
#include <exo/exo-string.h>
#include <exo/exo-alias.h>



#define MINIMUM_ICON_ITEM_WIDTH 32
#define ICON_TEXT_PADDING 1

#define EXO_ICON_BAR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_ICON_BAR, ExoIconBarPrivate))

#define EXO_ICON_BAR_VALID_MODEL_AND_COLUMNS(obj) ((obj)->priv->model != NULL && \
                                                   (obj)->priv->pixbuf_column != -1 && \
                                                   (obj)->priv->text_column != -1)



typedef struct _ExoIconBarItem ExoIconBarItem;

enum
{
  PROP_0,
  PROP_ORIENTATION,
  PROP_PIXBUF_COLUMN,
  PROP_TEXT_COLUMN,
  PROP_MODEL,
  PROP_ACTIVE,
};

enum
{
  SELECTION_CHANGED,
  LAST_SIGNAL,
};



static void            exo_icon_bar_class_init            (ExoIconBarClass  *klass);
static void            exo_icon_bar_init                  (ExoIconBar       *icon_bar);
static void            exo_icon_bar_destroy               (GtkObject        *object);
static void            exo_icon_bar_finalize              (GObject          *object);
static void            exo_icon_bar_get_property          (GObject          *object,
                                                           guint             prop_id,
                                                           GValue           *value,
                                                           GParamSpec       *pspec);
static void            exo_icon_bar_set_property          (GObject          *object,
                                                           guint             prop_id,
                                                           const GValue     *value,
                                                           GParamSpec       *pspec);
static void            exo_icon_bar_style_set             (GtkWidget        *widget,
                                                           GtkStyle         *previous_style);
static void            exo_icon_bar_realize               (GtkWidget        *widget);
static void            exo_icon_bar_unrealize             (GtkWidget        *widget);
static void            exo_icon_bar_size_request          (GtkWidget        *widget,
                                                           GtkRequisition   *requisition);
static void            exo_icon_bar_size_allocate         (GtkWidget        *widget,
                                                           GtkAllocation    *allocation);
static gboolean        exo_icon_bar_expose                (GtkWidget        *widget,
                                                           GdkEventExpose   *expose);
static gboolean        exo_icon_bar_leave                 (GtkWidget        *widget,
                                                           GdkEventCrossing *event);
static gboolean        exo_icon_bar_motion                (GtkWidget        *widget,
                                                           GdkEventMotion   *event);
static gboolean        exo_icon_bar_button_press          (GtkWidget        *widget,
                                                           GdkEventButton   *event);
static void            exo_icon_bar_set_adjustments       (ExoIconBar       *icon_bar,
                                                           GtkAdjustment    *hadj,
                                                           GtkAdjustment    *vadj);
static void            exo_icon_bar_adjustment_changed    (GtkAdjustment    *adjustment,
                                                           ExoIconBar       *icon_bar);
static void            exo_icon_bar_invalidate            (ExoIconBar       *icon_bar);
static ExoIconBarItem *exo_icon_bar_get_item_at_pos       (ExoIconBar       *icon_bar,
                                                           gint              x,
                                                           gint              y);
static void            exo_icon_bar_queue_draw_item       (ExoIconBar       *icon_bar,
                                                           ExoIconBarItem   *item);
static void            exo_icon_bar_paint_item            (ExoIconBar       *icon_bar,
                                                           ExoIconBarItem   *item,
                                                           GdkRectangle     *area);
static void            exo_icon_bar_calculate_item_size   (ExoIconBar       *icon_bar,
                                                           ExoIconBarItem   *item);
static void            exo_icon_bar_update_item_text      (ExoIconBar       *icon_bar,
                                                           ExoIconBarItem   *item);
static GdkPixbuf      *exo_icon_bar_get_item_icon         (ExoIconBar       *icon_bar,
                                                           ExoIconBarItem   *item);
static ExoIconBarItem *exo_icon_bar_item_new              (void);
static void            exo_icon_bar_item_free             (ExoIconBarItem   *item);
static void            exo_icon_bar_item_invalidate       (ExoIconBarItem   *item);
static void            exo_icon_bar_build_items           (ExoIconBar       *icon_bar);
static void            exo_icon_bar_row_changed           (GtkTreeModel     *model,
                                                           GtkTreePath      *path,
                                                           GtkTreeIter      *iter,
                                                           ExoIconBar       *icon_bar);
static void            exo_icon_bar_row_inserted          (GtkTreeModel     *model,
                                                           GtkTreePath      *path,
                                                           GtkTreeIter      *iter,
                                                           ExoIconBar       *icon_bar);
static void            exo_icon_bar_row_deleted           (GtkTreeModel     *model,
                                                           GtkTreePath      *path,
                                                           GtkTreeIter      *iter,
                                                           ExoIconBar       *icon_bar);
static void            exo_icon_bar_rows_reordered        (GtkTreeModel     *model,
                                                           GtkTreePath      *path,
                                                           GtkTreeIter      *iter,
                                                           gint             *new_order,
                                                           ExoIconBar       *icon_bar);



struct _ExoIconBarItem
{
  GtkTreeIter iter;
  gint        index;

  gint        width;
  gint        height;

  gint        pixbuf_width;
  gint        pixbuf_height;

  gint        layout_width;
  gint        layout_height;
};

struct _ExoIconBarPrivate
{
  GdkWindow      *bin_window;

  gint            width;
  gint            height;

  gint            pixbuf_column;
  gint            text_column;

  ExoIconBarItem *active_item;
  ExoIconBarItem *cursor_item;
  GList          *items;
  gint            item_width;
  gint            item_height;

  GtkAdjustment  *hadjustment;
  GtkAdjustment  *vadjustment;

  GtkOrientation  orientation;

  GtkTreeModel   *model;

  PangoLayout    *layout;
};



static GObjectClass *exo_icon_bar_parent_class;
static guint         icon_bar_signals[LAST_SIGNAL];



GType
exo_icon_bar_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ExoIconBarClass),
        NULL,
        NULL,
        (GClassInitFunc) exo_icon_bar_class_init,
        NULL,
        NULL,
        sizeof (ExoIconBar),
        0,
        (GInstanceInitFunc) exo_icon_bar_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_CONTAINER, I_("ExoIconBar"), &info, 0);
    }

  return type;
}



static void
exo_icon_bar_class_init (ExoIconBarClass *klass)
{
  GtkObjectClass *gtkobject_class;
  GtkWidgetClass *gtkwidget_class;
  GObjectClass   *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoIconBarPrivate));

  exo_icon_bar_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_icon_bar_finalize;
  gobject_class->get_property = exo_icon_bar_get_property;
  gobject_class->set_property = exo_icon_bar_set_property;

  gtkobject_class = GTK_OBJECT_CLASS (klass);
  gtkobject_class->destroy = exo_icon_bar_destroy;

  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->style_set = exo_icon_bar_style_set;
  gtkwidget_class->realize = exo_icon_bar_realize;
  gtkwidget_class->unrealize = exo_icon_bar_unrealize;
  gtkwidget_class->size_request = exo_icon_bar_size_request;
  gtkwidget_class->size_allocate = exo_icon_bar_size_allocate;
  gtkwidget_class->expose_event = exo_icon_bar_expose;
  gtkwidget_class->leave_notify_event = exo_icon_bar_leave;
  gtkwidget_class->motion_notify_event = exo_icon_bar_motion;
  gtkwidget_class->button_press_event = exo_icon_bar_button_press;

  klass->set_scroll_adjustments = exo_icon_bar_set_adjustments;

  /**
   * ExoIconBar:orientation:
   *
   * The orientation of the icon bar.
   *
   * Default value: %GTK_ORIENTATION_VERTICAL
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ORIENTATION,
                                   g_param_spec_enum ("orientation",
                                                      _("Orientation"),
                                                      _("The orientation of the iconbar"),
                                                      GTK_TYPE_ORIENTATION,
                                                      GTK_ORIENTATION_VERTICAL,
                                                      EXO_PARAM_READWRITE));

  /**
   * ExoIconBar:pixbuf-column:
   *
   * The ::pixbuf-column property contains the number of the model column
   * containing the pixbufs which are displyed. The pixbuf column must be
   * of type #GDK_TYPE_PIXBUF. Setting this property to -1 turns off the
   * display of pixbufs.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_PIXBUF_COLUMN,
                                   g_param_spec_int ("pixbuf-column",
                                                     _("Pixbuf column"),
                                                     _("Model column used to retrieve the icon pixbuf from"),
                                                     -1, G_MAXINT, -1,
                                                     EXO_PARAM_READWRITE));

  /**
   * ExoIconBar:text-column:
   *
   * The ::text-column property contains the number of the model column
   * containing the texts which are displayed. The text column must be
   * of type #G_TYPE_STRING. If this property is set to -1, no texts
   * are displayed.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT_COLUMN,
                                   g_param_spec_int ("text-column",
                                                     _("Text column"),
                                                     _("Model column used to retrieve the text from"),
                                                     -1, G_MAXINT, -1,
                                                     EXO_PARAM_READWRITE));

  /**
   * ExoIconBar:model:
   *
   * The model for the icon bar.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_MODEL,
                                   g_param_spec_object ("model",
                                                        _("Icon Bar Model"),
                                                        _("Model for the icon bar"),
                                                        GTK_TYPE_TREE_MODEL,
                                                        EXO_PARAM_READWRITE));

  /**
   * ExoIconBar:active:
   *
   * The item which is currently active.
   *
   * Allowed values: >= -1
   *
   * Default value: -1
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ACTIVE,
                                   g_param_spec_int ("active",
                                                     _("Active"),
                                                     _("Active item index"),
                                                     -1, G_MAXINT, -1,
                                                     EXO_PARAM_READWRITE));

  gtk_widget_class_install_style_property (gtkwidget_class,
                                           g_param_spec_boxed ("active-item-fill-color",
                                                               _("Active item fill color"),
                                                               _("Active item fill color"),
                                                               GDK_TYPE_COLOR,
                                                               EXO_PARAM_READABLE));

  gtk_widget_class_install_style_property (gtkwidget_class,
                                           g_param_spec_boxed ("active-item-border-color",
                                                               _("Active item border color"),
                                                               _("Active item border color"),
                                                               GDK_TYPE_COLOR,
                                                               EXO_PARAM_READABLE));

  gtk_widget_class_install_style_property (gtkwidget_class,
                                           g_param_spec_boxed ("active-item-text-color",
                                                               _("Active item text color"),
                                                               _("Active item text color"),
                                                               GDK_TYPE_COLOR,
                                                               EXO_PARAM_READABLE));

  gtk_widget_class_install_style_property (gtkwidget_class,
                                           g_param_spec_boxed ("cursor-item-fill-color",
                                                               _("Cursor item fill color"),
                                                               _("Cursor item fill color"),
                                                               GDK_TYPE_COLOR,
                                                               EXO_PARAM_READABLE));

  gtk_widget_class_install_style_property (gtkwidget_class,
                                           g_param_spec_boxed ("cursor-item-border-color",
                                                               _("Cursor item border color"),
                                                               _("Cursor item border color"),
                                                               GDK_TYPE_COLOR,
                                                               EXO_PARAM_READABLE));

  gtk_widget_class_install_style_property (gtkwidget_class,
                                           g_param_spec_boxed ("cursor-item-text-color",
                                                               _("Cursor item text color"),
                                                               _("Cursor item text color"),
                                                               GDK_TYPE_COLOR,
                                                               EXO_PARAM_READABLE));

  /**
   * ExoIconBar::set-scroll-adjustments:
   * @icon_bar    : The #ExoIconBar.
   * @hadjustment : The horizontal adjustment.
   * @vadjustment : The vertical adjustment.
   *
   * Used internally to make the #ExoIconBar scrollable.
   **/
  gtkwidget_class->set_scroll_adjustments_signal =
    g_signal_new (I_("set-scroll-adjustments"),
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ExoIconBarClass, set_scroll_adjustments),
                  NULL, NULL,
                  _exo_marshal_VOID__OBJECT_OBJECT,
                  G_TYPE_NONE, 2,
                  GTK_TYPE_ADJUSTMENT,
                  GTK_TYPE_ADJUSTMENT);

  /**
   * ExoIconBar::selection-changed:
   * @icon_bar  : The #ExoIconBar.
   *
   * This signal is emitted whenever the currently selected icon
   * changes.
   **/
  icon_bar_signals[SELECTION_CHANGED] =
    g_signal_new (I_("selection-changed"),
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (ExoIconBarClass, selection_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}



static void
exo_icon_bar_init (ExoIconBar *icon_bar)
{
  icon_bar->priv = EXO_ICON_BAR_GET_PRIVATE (icon_bar);

  icon_bar->priv->orientation = GTK_ORIENTATION_VERTICAL;
  icon_bar->priv->pixbuf_column = -1;
  icon_bar->priv->text_column = -1;

  icon_bar->priv->layout = gtk_widget_create_pango_layout (GTK_WIDGET (icon_bar), NULL);
  pango_layout_set_width (icon_bar->priv->layout, -1);

  GTK_WIDGET_UNSET_FLAGS (icon_bar, GTK_CAN_FOCUS);

  exo_icon_bar_set_adjustments (icon_bar, NULL, NULL);
}



static void
exo_icon_bar_destroy (GtkObject *object)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (object);

  exo_icon_bar_set_model (icon_bar, NULL);

  (*GTK_OBJECT_CLASS (exo_icon_bar_parent_class)->destroy) (object);
}



static void
exo_icon_bar_finalize (GObject *object)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (object);

  g_object_unref (G_OBJECT (icon_bar->priv->layout));

  (*G_OBJECT_CLASS (exo_icon_bar_parent_class)->finalize) (object);
}



static void
exo_icon_bar_get_property (GObject          *object,
                           guint             prop_id,
                           GValue           *value,
                           GParamSpec       *pspec)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (object);

  switch (prop_id)
    {
    case PROP_ORIENTATION:
      g_value_set_enum (value, icon_bar->priv->orientation);
      break;

    case PROP_PIXBUF_COLUMN:
      g_value_set_int (value, icon_bar->priv->pixbuf_column);
      break;

    case PROP_TEXT_COLUMN:
      g_value_set_int (value, icon_bar->priv->text_column);
      break;

    case PROP_MODEL:
      g_value_set_object (value, icon_bar->priv->model);
      break;

    case PROP_ACTIVE:
      g_value_set_int (value, exo_icon_bar_get_active (icon_bar));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_icon_bar_set_property (GObject          *object,
                           guint             prop_id,
                           const GValue     *value,
                           GParamSpec       *pspec)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (object);

  switch (prop_id)
    {
    case PROP_ORIENTATION:
      exo_icon_bar_set_orientation (icon_bar, g_value_get_enum (value));
      break;

    case PROP_PIXBUF_COLUMN:
      exo_icon_bar_set_pixbuf_column (icon_bar, g_value_get_int (value));
      break;

    case PROP_TEXT_COLUMN:
      exo_icon_bar_set_text_column (icon_bar, g_value_get_int (value));
      break;

    case PROP_MODEL:
      exo_icon_bar_set_model (icon_bar, g_value_get_object (value));
      break;

    case PROP_ACTIVE:
      exo_icon_bar_set_active (icon_bar, g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_icon_bar_style_set (GtkWidget *widget,
                        GtkStyle  *previous_style)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (widget);

  (*GTK_WIDGET_CLASS (exo_icon_bar_parent_class)->style_set) (widget, previous_style);

  if (GTK_WIDGET_REALIZED (widget))
    {
      gdk_window_set_background (icon_bar->priv->bin_window,
                                 &widget->style->base[widget->state]);
    }
}



static void
exo_icon_bar_realize (GtkWidget *widget)
{
  GdkWindowAttr attributes;
  ExoIconBar   *icon_bar = EXO_ICON_BAR (widget);
  gint          attributes_mask;

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = GDK_VISIBILITY_NOTIFY_MASK;
  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                   &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  attributes.x = 0;
  attributes.y = 0;
  attributes.width = MAX (icon_bar->priv->width, widget->allocation.width);
  attributes.height = MAX (icon_bar->priv->height, widget->allocation.height);
  attributes.event_mask = (GDK_SCROLL_MASK
                           | GDK_EXPOSURE_MASK
                           | GDK_LEAVE_NOTIFY_MASK
                           | GDK_POINTER_MOTION_MASK
                           | GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_KEY_PRESS_MASK
                           | GDK_KEY_RELEASE_MASK)
                          | gtk_widget_get_events (widget);
  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  icon_bar->priv->bin_window = gdk_window_new (widget->window,
                                               &attributes, attributes_mask);
  gdk_window_set_user_data (icon_bar->priv->bin_window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gdk_window_set_background (widget->window, &widget->style->base[widget->state]);
  gdk_window_set_background (icon_bar->priv->bin_window, &widget->style->base[widget->state]);
  gdk_window_show (icon_bar->priv->bin_window);
}



static void
exo_icon_bar_unrealize (GtkWidget *widget)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (widget);

  gdk_window_set_user_data (icon_bar->priv->bin_window, NULL);
  gdk_window_destroy (icon_bar->priv->bin_window);
  icon_bar->priv->bin_window = NULL;

  /* GtkWidget::unrealize destroys children and widget->window */
  (*GTK_WIDGET_CLASS (exo_icon_bar_parent_class)->unrealize) (widget);
}



static void
exo_icon_bar_size_request (GtkWidget      *widget,
                           GtkRequisition *requisition)
{
  ExoIconBarItem *item;
  ExoIconBar     *icon_bar = EXO_ICON_BAR (widget);
  GList          *lp;
  gint            n = 0;
  gint            max_width = 0;
  gint            max_height = 0;

  if (!EXO_ICON_BAR_VALID_MODEL_AND_COLUMNS (icon_bar)
      || icon_bar->priv->items == NULL)
    {
      icon_bar->priv->width = requisition->width = 0;
      icon_bar->priv->height = requisition->height = 0;
      return;
    }

  /* calculate max item size */
  for (lp = icon_bar->priv->items; lp != NULL; ++n, lp = lp->next)
    {
      item = lp->data;
      exo_icon_bar_calculate_item_size (icon_bar, item);

      if (item->width > max_width)
        max_width = item->width;
      if (item->height > max_height)
        max_height = item->height;
    }

  icon_bar->priv->item_width = max_width;
  icon_bar->priv->item_height = max_height;

  if (icon_bar->priv->orientation == GTK_ORIENTATION_VERTICAL)
    {
      icon_bar->priv->width = requisition->width = icon_bar->priv->item_width;
      icon_bar->priv->height = requisition->height = icon_bar->priv->item_height * n;
    }
  else
    {
      icon_bar->priv->width = requisition->width = icon_bar->priv->item_width * n;
      icon_bar->priv->height = requisition->height = icon_bar->priv->item_height;
    }
}



static void
exo_icon_bar_size_allocate (GtkWidget     *widget,
                            GtkAllocation *allocation)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (widget);

  widget->allocation = *allocation;

  if (GTK_WIDGET_REALIZED (widget))
    {
      gdk_window_move_resize (widget->window,
                              allocation->x,
                              allocation->y,
                              allocation->width,
                              allocation->height);
      gdk_window_resize (icon_bar->priv->bin_window,
                         MAX (icon_bar->priv->width, allocation->width),
                         MAX (icon_bar->priv->height, allocation->height));
    }

  icon_bar->priv->hadjustment->page_size = allocation->width;
  icon_bar->priv->hadjustment->page_increment = allocation->width * 0.9;
  icon_bar->priv->hadjustment->step_increment = allocation->width * 0.1;
  icon_bar->priv->hadjustment->lower = 0;
  icon_bar->priv->hadjustment->upper = MAX (allocation->width, icon_bar->priv->width);
  gtk_adjustment_changed (icon_bar->priv->hadjustment);

  icon_bar->priv->vadjustment->page_size = allocation->height;
  icon_bar->priv->vadjustment->page_increment = allocation->height * 0.9;
  icon_bar->priv->vadjustment->step_increment = allocation->height * 0.1;
  icon_bar->priv->vadjustment->lower = 0;
  icon_bar->priv->vadjustment->upper = MAX (allocation->height, icon_bar->priv->height);
  gtk_adjustment_changed (icon_bar->priv->vadjustment);

  if (icon_bar->priv->orientation == GTK_ORIENTATION_VERTICAL)
    {
      icon_bar->priv->width = MAX (icon_bar->priv->width, allocation->width);
      icon_bar->priv->item_width = icon_bar->priv->width;
    }
  else
    {
      icon_bar->priv->height = MAX (icon_bar->priv->height, allocation->height);
      icon_bar->priv->item_height = icon_bar->priv->height;
    }
}



static gboolean
exo_icon_bar_expose (GtkWidget      *widget,
                     GdkEventExpose *expose)
{
  ExoIconBarItem *item;
  GdkRectangle    area;
  ExoIconBar     *icon_bar = EXO_ICON_BAR (widget);
  GList          *lp;

  if (expose->window != icon_bar->priv->bin_window)
    return FALSE;

  for (lp = icon_bar->priv->items; lp != NULL; lp = lp->next)
    {
      item = lp->data;

      if (icon_bar->priv->orientation == GTK_ORIENTATION_VERTICAL)
        {
          area.x = 0;
          area.y = item->index * icon_bar->priv->item_height;
        }
      else
        {
          area.x = item->index * icon_bar->priv->item_width;
          area.y = 0;
        }

      area.width = icon_bar->priv->item_width;
      area.height = icon_bar->priv->item_height;

      if (gdk_region_rect_in (expose->region, &area) != GDK_OVERLAP_RECTANGLE_OUT)
        exo_icon_bar_paint_item (icon_bar, item, &expose->area);
    }

  return TRUE;
}



static gboolean
exo_icon_bar_leave (GtkWidget        *widget,
                    GdkEventCrossing *event)
{
  ExoIconBar *icon_bar = EXO_ICON_BAR (widget);

  if (icon_bar->priv->cursor_item != NULL)
    {
      exo_icon_bar_queue_draw_item (icon_bar, icon_bar->priv->cursor_item);
      icon_bar->priv->cursor_item = NULL;
    }

  return FALSE;
}



static gboolean
exo_icon_bar_motion (GtkWidget      *widget,
                     GdkEventMotion *event)
{
  ExoIconBarItem *item;
  ExoIconBar     *icon_bar = EXO_ICON_BAR (widget);

  item = exo_icon_bar_get_item_at_pos (icon_bar, event->x, event->y);
  if (item != NULL && icon_bar->priv->cursor_item != item)
    {
      if (icon_bar->priv->cursor_item != NULL)
        exo_icon_bar_queue_draw_item (icon_bar, icon_bar->priv->cursor_item);
      icon_bar->priv->cursor_item = item;
      exo_icon_bar_queue_draw_item (icon_bar, item);
    }
  else if (icon_bar->priv->cursor_item != NULL
        && icon_bar->priv->cursor_item != item)
    {
      exo_icon_bar_queue_draw_item (icon_bar, icon_bar->priv->cursor_item);
      icon_bar->priv->cursor_item = NULL;
    }

  return TRUE;
}



static gboolean
exo_icon_bar_button_press (GtkWidget      *widget,
                           GdkEventButton *event)
{
  ExoIconBarItem  *item;
  ExoIconBar      *icon_bar = EXO_ICON_BAR (widget);

  if (!GTK_WIDGET_HAS_FOCUS (widget))
    gtk_widget_grab_focus (widget);

  if (event->button == 1 && event->type == GDK_BUTTON_PRESS)
    {
      item = exo_icon_bar_get_item_at_pos (icon_bar, event->x, event->y);
      if (G_LIKELY (item != NULL && item != icon_bar->priv->active_item))
        exo_icon_bar_set_active (icon_bar, item->index);
    }

  return TRUE;
}



static void
exo_icon_bar_set_adjustments (ExoIconBar    *icon_bar,
                              GtkAdjustment *hadj,
                              GtkAdjustment *vadj)
{
  gboolean need_adjust = FALSE;

  if (hadj != NULL)
    g_return_if_fail (GTK_IS_ADJUSTMENT (hadj));
  else
    hadj = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));

  if (vadj != NULL)
    g_return_if_fail (GTK_IS_ADJUSTMENT (vadj));
  else
    vadj = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));

  if (icon_bar->priv->hadjustment && (icon_bar->priv->hadjustment != hadj))
    {
      g_signal_handlers_disconnect_matched (icon_bar->priv->hadjustment, G_SIGNAL_MATCH_DATA,
                                            0, 0, NULL, NULL, icon_bar);
      g_object_unref (icon_bar->priv->hadjustment);
    }

  if (icon_bar->priv->vadjustment && (icon_bar->priv->vadjustment != vadj))
    {
      g_signal_handlers_disconnect_matched (icon_bar->priv->vadjustment, G_SIGNAL_MATCH_DATA,
                                            0, 0, NULL, NULL, icon_bar);
      g_object_unref (icon_bar->priv->vadjustment);
    }

  if (icon_bar->priv->hadjustment != hadj)
    {
      icon_bar->priv->hadjustment = hadj;
      g_object_ref (icon_bar->priv->hadjustment);
      gtk_object_sink (GTK_OBJECT (icon_bar->priv->hadjustment));

      g_signal_connect (icon_bar->priv->hadjustment, "value_changed",
                        G_CALLBACK (exo_icon_bar_adjustment_changed), icon_bar);
      need_adjust = TRUE;
    }

  if (icon_bar->priv->vadjustment != vadj)
    {
      icon_bar->priv->vadjustment = vadj;
      g_object_ref (icon_bar->priv->vadjustment);
      gtk_object_sink (GTK_OBJECT (icon_bar->priv->vadjustment));

      g_signal_connect (icon_bar->priv->vadjustment, "value_changed",
			                  G_CALLBACK (exo_icon_bar_adjustment_changed), icon_bar);
      need_adjust = TRUE;
    }

  if (need_adjust)
    exo_icon_bar_adjustment_changed (NULL, icon_bar);
}



static void
exo_icon_bar_adjustment_changed (GtkAdjustment *adjustment,
                                 ExoIconBar    *icon_bar)
{
  if (GTK_WIDGET_REALIZED (icon_bar))
    {
      gdk_window_move (icon_bar->priv->bin_window,
                       - icon_bar->priv->hadjustment->value,
                       - icon_bar->priv->vadjustment->value);

      gdk_window_process_updates (icon_bar->priv->bin_window, TRUE);
    }
}



static void
exo_icon_bar_invalidate (ExoIconBar *icon_bar)
{
  g_list_foreach (icon_bar->priv->items, (GFunc) exo_icon_bar_item_invalidate, NULL);
  gtk_widget_queue_resize (GTK_WIDGET (icon_bar));
}



static ExoIconBarItem*
exo_icon_bar_get_item_at_pos (ExoIconBar *icon_bar,
                              gint        x,
                              gint        y)
{
  GList *lp;

  if (G_UNLIKELY (icon_bar->priv->item_height == 0))
    return NULL;

  if (icon_bar->priv->orientation == GTK_ORIENTATION_VERTICAL)
    lp = g_list_nth (icon_bar->priv->items, y / icon_bar->priv->item_height);
  else
    lp = g_list_nth (icon_bar->priv->items, x / icon_bar->priv->item_width);

  return (lp != NULL) ? lp->data : NULL;
}



static void
exo_icon_bar_queue_draw_item (ExoIconBar     *icon_bar,
                              ExoIconBarItem *item)
{
  GdkRectangle area;

  if (GTK_WIDGET_REALIZED (icon_bar))
    {
      if (icon_bar->priv->orientation == GTK_ORIENTATION_VERTICAL)
        {
          area.x = 0;
          area.y = icon_bar->priv->item_height * item->index;
        }
      else
        {
          area.x = icon_bar->priv->item_width * item->index;
          area.y = 0;
        }

      area.width = icon_bar->priv->item_width;
      area.height = icon_bar->priv->item_height;

      gdk_window_invalidate_rect (icon_bar->priv->bin_window, &area, TRUE);
    }
}



static void
exo_icon_bar_paint_item (ExoIconBar     *icon_bar,
                         ExoIconBarItem *item,
                         GdkRectangle   *area)
{
  GdkPixbuf    *pixbuf;
  GdkColor     *border_color;
  GdkColor     *fill_color;
  GdkColor     *text_color;
  GdkGC        *gc;
  gint          focus_width;
  gint          focus_pad;
  gint          x, y;
  gint          px, py;
  gint          lx, ly;

  if (!EXO_ICON_BAR_VALID_MODEL_AND_COLUMNS (icon_bar))
    return;

  gtk_widget_style_get (GTK_WIDGET (icon_bar),
                        "focus-line-width", &focus_width,
                        "focus-padding", &focus_pad,
                        NULL);

  /* calculate pixbuf/layout location */
  if (icon_bar->priv->orientation == GTK_ORIENTATION_VERTICAL)
    {
      x = 0;
      y = icon_bar->priv->item_height * item->index;

      px = (icon_bar->priv->item_width - item->pixbuf_width) / 2 + focus_pad + focus_width;
      py = (icon_bar->priv->item_height - (item->pixbuf_height + item->layout_height + ICON_TEXT_PADDING)) / 2
         + icon_bar->priv->item_height * item->index + focus_pad + focus_width;
      lx = (icon_bar->priv->item_width - (item->layout_width + ICON_TEXT_PADDING)) / 2 + focus_pad;
      ly = py + item->pixbuf_height + ICON_TEXT_PADDING;
    }
  else
    {
      x = icon_bar->priv->item_width * item->index;
      y = 0;

      px = (icon_bar->priv->item_width - item->pixbuf_width) / 2 + focus_pad + focus_width
         + icon_bar->priv->item_width * item->index;
      py = (icon_bar->priv->item_height - (item->pixbuf_height + item->layout_height)) / 2
          + focus_pad + focus_width;
      lx = (icon_bar->priv->item_width - (item->layout_width)) / 2 + x;
      ly = py + item->pixbuf_height + ICON_TEXT_PADDING;
    }

  if (icon_bar->priv->active_item == item)
    {
      gtk_widget_style_get (GTK_WIDGET (icon_bar),
                            "active-item-fill-color", &fill_color,
                            "active-item-border-color", &border_color,
                            NULL);

      if (fill_color == NULL)
        {
          fill_color = gdk_color_copy (&GTK_WIDGET (icon_bar)->style->base[GTK_STATE_SELECTED]);
          gdk_color_parse ("#c1d2ee", fill_color);
        }

      if (border_color == NULL)
        {
          border_color = gdk_color_copy (&GTK_WIDGET (icon_bar)->style->base[GTK_STATE_SELECTED]);
          gdk_color_parse ("#316ac5", border_color);
        }

      gc = gdk_gc_new (icon_bar->priv->bin_window);
      gdk_gc_set_clip_rectangle (gc, area);
      gdk_gc_set_rgb_fg_color (gc, fill_color);
      gdk_draw_rectangle (icon_bar->priv->bin_window, gc, TRUE,
                          x + focus_pad + focus_width,
                          y + focus_pad + focus_width,
                          icon_bar->priv->item_width - 2 * (focus_width + focus_pad) + 1,
                          icon_bar->priv->item_height - 2 * (focus_width + focus_pad) + 1);
      gdk_gc_set_rgb_fg_color (gc, border_color);
      gdk_gc_set_line_attributes (gc, focus_width, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
      gdk_draw_rectangle (icon_bar->priv->bin_window, gc, FALSE,
                          x + focus_pad + focus_width / 2,
                          y + focus_pad + focus_width / 2,
                          icon_bar->priv->item_width - (2 * focus_pad + focus_width) + 1,
                          icon_bar->priv->item_height - (2 * focus_pad + focus_width) + 1);
      gdk_color_free (border_color);
      gdk_color_free (fill_color);
      g_object_unref (gc);
    }
  else if (icon_bar->priv->cursor_item == item)
    {
      gtk_widget_style_get (GTK_WIDGET (icon_bar),
                            "cursor-item-fill-color", &fill_color,
                            "cursor-item-border-color", &border_color,
                            NULL);

      if (fill_color == NULL)
        {
          fill_color = gdk_color_copy (&GTK_WIDGET (icon_bar)->style->base[GTK_STATE_SELECTED]);
          gdk_color_parse ("#e0e8f6", fill_color);
        }

      if (border_color == NULL)
        {
          border_color = gdk_color_copy (&GTK_WIDGET (icon_bar)->style->base[GTK_STATE_SELECTED]);
          gdk_color_parse ("#98b4e2", border_color);
        }

      gc = gdk_gc_new (icon_bar->priv->bin_window);
      gdk_gc_set_clip_rectangle (gc, area);
      gdk_gc_set_rgb_fg_color (gc, fill_color);
      gdk_draw_rectangle (icon_bar->priv->bin_window, gc, TRUE,
                          x + focus_pad + focus_width,
                          y + focus_pad + focus_width,
                          icon_bar->priv->item_width - 2 * (focus_width + focus_pad) + 1,
                          icon_bar->priv->item_height - 2 * (focus_width + focus_pad) + 1);
      gdk_gc_set_rgb_fg_color (gc, border_color);
      gdk_gc_set_line_attributes (gc, focus_width, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
      gdk_draw_rectangle (icon_bar->priv->bin_window, gc, FALSE,
                          x + focus_pad + focus_width / 2,
                          y + focus_pad + focus_width / 2,
                          icon_bar->priv->item_width - (2 * focus_pad + focus_width) + 1,
                          icon_bar->priv->item_height - (2 * focus_pad + focus_width) + 1);
      gdk_color_free (border_color);
      gdk_color_free (fill_color);
      g_object_unref (gc);
    }

  if (icon_bar->priv->pixbuf_column != -1)
    {
      pixbuf = exo_icon_bar_get_item_icon (icon_bar, item);
      if (G_LIKELY (pixbuf != NULL))
        {
          gdk_draw_pixbuf (icon_bar->priv->bin_window, NULL, pixbuf, 0, 0,
                           px, py, item->pixbuf_width, item->pixbuf_height,
                           GDK_RGB_DITHER_NORMAL, item->pixbuf_width,
                           item->pixbuf_height);
          g_object_unref (pixbuf);
        }
    }

  if (icon_bar->priv->text_column != -1)
    {
      exo_icon_bar_update_item_text (icon_bar, item);

      if (icon_bar->priv->active_item == item)
        {
          gtk_widget_style_get (GTK_WIDGET (icon_bar),
                                "active-item-text-color", &text_color,
                                NULL);

          if (text_color == NULL)
            {
              text_color = gdk_color_copy (&GTK_WIDGET (icon_bar)->style->base[GTK_STATE_SELECTED]);
              gdk_color_parse ("#000000", text_color);
            }

          gc = gdk_gc_new (GDK_DRAWABLE (icon_bar->priv->bin_window));
          gdk_gc_copy (gc, GTK_WIDGET (icon_bar)->style->text_gc[GTK_STATE_SELECTED]);
          gdk_gc_set_clip_rectangle (gc, area);
          gdk_gc_set_rgb_fg_color (gc, text_color);
          gdk_draw_layout (icon_bar->priv->bin_window, gc, lx, ly, icon_bar->priv->layout);
          g_object_unref (G_OBJECT (gc));
          gdk_color_free (text_color);
        }
      else if (icon_bar->priv->cursor_item == item)
        {
          gtk_widget_style_get (GTK_WIDGET (icon_bar),
                                "cursor-item-text-color", &text_color,
                                NULL);

          if (text_color == NULL)
            {
              text_color = gdk_color_copy (&GTK_WIDGET (icon_bar)->style->base[GTK_STATE_SELECTED]);
              gdk_color_parse ("#000000", text_color);
            }

          gc = gdk_gc_new (GDK_DRAWABLE (icon_bar->priv->bin_window));
          gdk_gc_copy (gc, GTK_WIDGET (icon_bar)->style->text_gc[GTK_STATE_SELECTED]);
          gdk_gc_set_clip_rectangle (gc, area);
          gdk_gc_set_rgb_fg_color (gc, text_color);
          gdk_draw_layout (icon_bar->priv->bin_window, gc, lx, ly, icon_bar->priv->layout);
          g_object_unref (G_OBJECT (gc));
          gdk_color_free (text_color);
        }
      else
        {
          gtk_paint_layout (GTK_WIDGET (icon_bar)->style,
                            icon_bar->priv->bin_window,
                            GTK_STATE_NORMAL, TRUE, area,
                            GTK_WIDGET (icon_bar), "icon_bar",
                            lx, ly, icon_bar->priv->layout);
        }
    }
}



static void
exo_icon_bar_calculate_item_size (ExoIconBar      *icon_bar,
                                  ExoIconBarItem  *item)
{
  GdkPixbuf *pixbuf;
  gint       focus_width;
  gint       focus_pad;

  if (G_LIKELY (item->width != -1 && item->width != -1))
    return;

  gtk_widget_style_get (GTK_WIDGET (icon_bar),
                        "focus-line-width", &focus_width,
                        "focus-padding", &focus_pad,
                        NULL);

  if (icon_bar->priv->pixbuf_column != -1)
    {
      pixbuf = exo_icon_bar_get_item_icon (icon_bar, item);
      if (G_LIKELY (pixbuf != NULL))
        {
          item->pixbuf_width = gdk_pixbuf_get_width (pixbuf);
          item->pixbuf_height = gdk_pixbuf_get_height (pixbuf);
          g_object_unref (G_OBJECT (pixbuf));
        }
      else
        {
          item->pixbuf_width = 0;
          item->pixbuf_height = 0;
        }
    }
  else
    {
      item->pixbuf_width = 0;
      item->pixbuf_height = 0;
    }

  if (icon_bar->priv->text_column != -1)
    {
      exo_icon_bar_update_item_text (icon_bar, item);
      pango_layout_get_pixel_size (icon_bar->priv->layout,
                                   &item->layout_width,
                                   &item->layout_height);
    }
  else
    {
      item->layout_width = 0;
      item->layout_height = 0;
    }

  item->width = MAX (item->layout_width, item->pixbuf_width) + 2 * ICON_TEXT_PADDING
              + 2 * (focus_width + focus_pad);
  item->height = item->layout_height + 2 * (focus_width + focus_pad + ICON_TEXT_PADDING)
               + item->pixbuf_height;
}



static void
exo_icon_bar_update_item_text (ExoIconBar     *icon_bar,
                               ExoIconBarItem *item)
{
  GtkTreePath *path;
  GtkTreeIter  iter;
  gchar       *text;

  if ((gtk_tree_model_get_flags (icon_bar->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST) == 0)
    {
      path = gtk_tree_path_new_from_indices (item->index, -1);
      gtk_tree_model_get_iter (icon_bar->priv->model, &iter, path);
      gtk_tree_path_free (path);
    }
  else
    {
      iter = item->iter;
    }

  gtk_tree_model_get (icon_bar->priv->model, &iter,
                      icon_bar->priv->text_column, &text,
                      -1);
  pango_layout_set_text (icon_bar->priv->layout, text, -1);
  g_free (text);
}



static GdkPixbuf*
exo_icon_bar_get_item_icon (ExoIconBar      *icon_bar,
                            ExoIconBarItem  *item)
{
  GtkTreePath *path;
  GtkTreeIter  iter;
  GdkPixbuf   *pixbuf;

  if ((gtk_tree_model_get_flags (icon_bar->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST) == 0)
    {
      path = gtk_tree_path_new_from_indices (item->index, -1);
      gtk_tree_model_get_iter (icon_bar->priv->model, &iter, path);
      gtk_tree_path_free (path);
    }
  else
    {
      iter = item->iter;
    }

  gtk_tree_model_get (icon_bar->priv->model, &iter,
                      icon_bar->priv->pixbuf_column, &pixbuf,
                      -1);

  return pixbuf;
}



static ExoIconBarItem*
exo_icon_bar_item_new (void)
{
  ExoIconBarItem *item;

  item = g_new0 (ExoIconBarItem, 1);
  item->width = -1;
  item->height = -1;

  return item;
}



static void
exo_icon_bar_item_free (ExoIconBarItem *item)
{
  g_free (item);
}



static void
exo_icon_bar_item_invalidate (ExoIconBarItem *item)
{
  item->width = -1;
  item->height = -1;
}



static void
exo_icon_bar_build_items (ExoIconBar *icon_bar)
{
  ExoIconBarItem *item;
  GtkTreeIter     iter;
  GList          *items = NULL;
  gint            i = 0;

  if (!gtk_tree_model_get_iter_first (icon_bar->priv->model, &iter))
    return;

  do
    {
      item = exo_icon_bar_item_new ();
      item->iter = iter;
      item->index = i++;

      items = g_list_prepend (items, item);
    }
  while (gtk_tree_model_iter_next (icon_bar->priv->model, &iter));

  icon_bar->priv->items = g_list_reverse (items);
}



static void
exo_icon_bar_row_changed (GtkTreeModel *model,
                          GtkTreePath  *path,
                          GtkTreeIter  *iter,
                          ExoIconBar   *icon_bar)
{
  ExoIconBarItem  *item;
  gint             index;

  index = gtk_tree_path_get_indices (path)[0];
  item = g_list_nth (icon_bar->priv->items, index)->data;
  exo_icon_bar_item_invalidate (item);
  gtk_widget_queue_resize (GTK_WIDGET (icon_bar));
}



static void
exo_icon_bar_row_inserted (GtkTreeModel *model,
                           GtkTreePath  *path,
                           GtkTreeIter  *iter,
                           ExoIconBar   *icon_bar)
{
  ExoIconBarItem  *item;
  GList           *lp;
  gint             index;

  index = gtk_tree_path_get_indices (path)[0];
  item = exo_icon_bar_item_new ();

  if ((gtk_tree_model_get_flags (icon_bar->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST) != 0)
    item->iter = *iter;
  item->index = index;

  icon_bar->priv->items = g_list_insert (icon_bar->priv->items, item, index);

  for (lp = g_list_nth (icon_bar->priv->items, index + 1); lp != NULL; lp = lp->next)
    {
      item = lp->data;
      item->index++;
    }

  gtk_widget_queue_resize (GTK_WIDGET (icon_bar));
}



static void
exo_icon_bar_row_deleted (GtkTreeModel *model,
                          GtkTreePath  *path,
                          GtkTreeIter  *iter,
                          ExoIconBar   *icon_bar)
{
  ExoIconBarItem *item;
  gboolean        active = FALSE;
  GList          *lnext;
  GList          *lp;
  gint            index;

  index = gtk_tree_path_get_indices (path)[0];
  lp = g_list_nth (icon_bar->priv->items, index);
  item = lp->data;

  if (item == icon_bar->priv->active_item)
    {
      icon_bar->priv->active_item = NULL;
      active = TRUE;
    }

  if (item == icon_bar->priv->cursor_item)
    icon_bar->priv->cursor_item = NULL;

  exo_icon_bar_item_free (item);

  for (lnext = lp->next; lnext != NULL; lnext = lnext->next)
    {
      item = lnext->data;
      item->index--;
    }

  icon_bar->priv->items = g_list_delete_link (icon_bar->priv->items, lp);

  if (active && icon_bar->priv->items != NULL)
    icon_bar->priv->active_item = icon_bar->priv->items->data;

  gtk_widget_queue_resize (GTK_WIDGET (icon_bar));

  if (active)
    exo_icon_bar_set_active (icon_bar, -1);
}



static void
exo_icon_bar_rows_reordered (GtkTreeModel *model,
                             GtkTreePath  *path,
                             GtkTreeIter  *iter,
                             gint         *new_order,
                             ExoIconBar   *icon_bar)
{
  ExoIconBarItem **item_array;
  GList           *items = NULL;
  GList           *lp;
  gint            *inverted_order;
  gint             length;
  gint             i;

  length = gtk_tree_model_iter_n_children (model, NULL);
  inverted_order = g_newa (gint, length);

  /* invert the array */
  for (i = 0; i < length; ++i)
    inverted_order[new_order[i]] = i;

  item_array = g_newa (ExoIconBarItem *, length);
  for (i = 0, lp = icon_bar->priv->items; lp != NULL; ++i, lp = lp->next)
    item_array[inverted_order[i]] = lp->data;

  for (i = 0; i < length; ++i)
    {
      item_array[i]->index = i;
      items = g_list_append (items, item_array[i]);
    }

  g_list_free (icon_bar->priv->items);
  icon_bar->priv->items = g_list_reverse (items);

  gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
}



/**
 * exo_icon_bar_new:
 *
 * Return value: a newly allocated #ExoIconBar.
 **/
GtkWidget*
exo_icon_bar_new (void)
{
  return g_object_new (EXO_TYPE_ICON_BAR, NULL);
}



/**
 * exo_icon_bar_new_with_model:
 * @model : A #GtkTreeModel.
 *
 * Creates a new #ExoIconBar and associates it with
 * @model.
 *
 * Return value: a newly allocated #ExoIconBar, which
 *               is associated with @model.
 **/
GtkWidget*
exo_icon_bar_new_with_model (GtkTreeModel *model)
{
  g_return_val_if_fail (GTK_IS_TREE_MODEL (model), NULL);

  return g_object_new (EXO_TYPE_ICON_BAR,
                       "model", model,
                       NULL);
}



/**
 * exo_icon_bar_get_model:
 * @icon_bar  : A #ExoIconBar.
 *
 * Returns the model the #ExoIconBar is based on. Returns %NULL if
 * the model is unset.
 * 
 * Return value: A #GtkTreeModel, or %NULL if none is currently being used.
 **/
GtkTreeModel*
exo_icon_bar_get_model (ExoIconBar *icon_bar)
{
  g_return_val_if_fail (EXO_IS_ICON_BAR (icon_bar), NULL);
  return icon_bar->priv->model;
}



/**
 * exo_icon_bar_set_model:
 * @icon_bar  : A #ExoIconBar.
 * @model     : A #GtkTreeModel or %NULL.
 *
 * Sets the model for a #ExoIconBar. If the @icon_bar already has a model
 * set, it will remove it before settings the new model. If @model is %NULL,
 * then it will unset the old model.
 **/
void
exo_icon_bar_set_model (ExoIconBar    *icon_bar,
                        GtkTreeModel  *model)
{
  GType pixbuf_column_type;
  GType text_column_type;
  gint  active = -1;

  g_return_if_fail (EXO_IS_ICON_BAR (icon_bar));
  g_return_if_fail (GTK_IS_TREE_MODEL (model) || model == NULL);

  if (G_UNLIKELY (model == icon_bar->priv->model))
    return;

  if (model != NULL)
    {
      g_return_if_fail (gtk_tree_model_get_flags (model) & GTK_TREE_MODEL_LIST_ONLY);

      if (icon_bar->priv->pixbuf_column != -1)
        {
          pixbuf_column_type = gtk_tree_model_get_column_type (model, icon_bar->priv->pixbuf_column);
          g_return_if_fail (pixbuf_column_type == GDK_TYPE_PIXBUF);
        }

      if (icon_bar->priv->text_column != -1)
        {
          text_column_type = gtk_tree_model_get_column_type (model, icon_bar->priv->text_column);
          g_return_if_fail (text_column_type == G_TYPE_STRING);
        }
    }

  if (icon_bar->priv->model)
    {
      g_signal_handlers_disconnect_by_func (icon_bar->priv->model,
                                            exo_icon_bar_row_changed,
                                            icon_bar);
      g_signal_handlers_disconnect_by_func (icon_bar->priv->model,
                                            exo_icon_bar_row_inserted,
                                            icon_bar);
      g_signal_handlers_disconnect_by_func (icon_bar->priv->model,
                                            exo_icon_bar_row_deleted,
                                            icon_bar);
      g_signal_handlers_disconnect_by_func (icon_bar->priv->model,
                                            exo_icon_bar_rows_reordered,
                                            icon_bar);

      g_object_unref (G_OBJECT (icon_bar->priv->model));

      g_list_foreach (icon_bar->priv->items, (GFunc) exo_icon_bar_item_free, NULL);
      g_list_free (icon_bar->priv->items);
      icon_bar->priv->active_item = NULL;
      icon_bar->priv->cursor_item = NULL;
      icon_bar->priv->items = NULL;
    }

  icon_bar->priv->model = model;

  if (model != NULL)
    {
      g_object_ref (G_OBJECT (model));

      g_signal_connect (G_OBJECT (model), "row-changed",
                        G_CALLBACK (exo_icon_bar_row_changed), icon_bar);
      g_signal_connect (G_OBJECT (model), "row-inserted",
                        G_CALLBACK (exo_icon_bar_row_inserted), icon_bar);
      g_signal_connect (G_OBJECT (model), "row-deleted",
                        G_CALLBACK (exo_icon_bar_row_deleted), icon_bar);
      g_signal_connect (G_OBJECT (model), "rows-reordered",
                        G_CALLBACK (exo_icon_bar_rows_reordered), icon_bar);

      exo_icon_bar_build_items (icon_bar);

      if (icon_bar->priv->items != NULL)
        active = ((ExoIconBarItem *) icon_bar->priv->items->data)->index;
    }

  exo_icon_bar_invalidate (icon_bar);

  g_object_notify (G_OBJECT (icon_bar), "model");

  exo_icon_bar_set_active (icon_bar, active);
}



/**
 * exo_icon_bar_get_pixbuf_column:
 * @icon_bar  : An #ExoIconBar.
 *
 * Returns the column with pixbufs for @icon_bar.
 *
 * Return value: the pixbuf column, or -1 if it's unset.
 **/
gint
exo_icon_bar_get_pixbuf_column (ExoIconBar *icon_bar)
{
  g_return_val_if_fail (EXO_IS_ICON_BAR (icon_bar), -1);
  return icon_bar->priv->pixbuf_column;
}



/**
 * exo_icon_bar_set_pixbuf_column:
 * @icon_bar  : An #ExoIconBar.
 * @column    : A column in the currently used model.
 *
 * Sets the column with pixbufs for @icon_bar to be @column. The pixbuf
 * column must be of type #GDK_TYPE_PIXBUF.
 **/
void
exo_icon_bar_set_pixbuf_column (ExoIconBar *icon_bar,
                                gint        column)
{
  GType pixbuf_column_type;

  g_return_if_fail (EXO_IS_ICON_BAR (icon_bar));

  if (column == icon_bar->priv->pixbuf_column)
    return;

  if (column == -1)
    {
      icon_bar->priv->pixbuf_column = -1;
    }
  else
    {
      if (icon_bar->priv->model != NULL)
        {
          pixbuf_column_type = gtk_tree_model_get_column_type (icon_bar->priv->model, column);
          g_return_if_fail (pixbuf_column_type == GDK_TYPE_PIXBUF);
        }

      icon_bar->priv->pixbuf_column = column;
    }

  exo_icon_bar_invalidate (icon_bar);

  g_object_notify (G_OBJECT (icon_bar), "pixbuf-column");
}



/**
 * exo_icon_bar_get_text_column:
 * @icon_bar  : An #ExoIconBar.
 *
 * Returns the column with text for @icon_bar.
 *
 * Return value: the text column, or -1 if it's unset.
 **/
gint
exo_icon_bar_get_text_column (ExoIconBar *icon_bar)
{
  g_return_val_if_fail (EXO_IS_ICON_BAR (icon_bar), -1);
  return icon_bar->priv->text_column;
}



/**
 * exo_icon_bar_set_text_column:
 * @icon_bar  : An #ExoIconBar.
 * @column    : A column in the currently used model or -1 to
 *              use no text in @icon_bar.
 *
 * Sets the column with text for @icon_bar to be @column. The
 * text column must be of type #G_TYPE_STRING.
 **/
void
exo_icon_bar_set_text_column (ExoIconBar *icon_bar,
                              gint        column)
{
  GType text_column_type;

  g_return_if_fail (EXO_IS_ICON_BAR (icon_bar));

  if (column == icon_bar->priv->text_column)
    return;

  if (column == -1)
    {
      icon_bar->priv->text_column = -1;
    }
  else
    {
      if (icon_bar->priv->model != NULL)
        {
          text_column_type = gtk_tree_model_get_column_type (icon_bar->priv->model, column);
          g_return_if_fail (text_column_type == G_TYPE_STRING);
        }

      icon_bar->priv->text_column = column;
    }

  exo_icon_bar_invalidate (icon_bar);

  g_object_notify (G_OBJECT (icon_bar), "text-column");
}



/**
 * exo_icon_bar_get_orientation:
 * @icon_bar  : An #ExoIconBar.
 *
 * Retrieves the current orientation of the toolbar. See
 * exo_icon_bar_set_orientation().
 *
 * Return value: The orientation of @icon_bar.
 **/
GtkOrientation
exo_icon_bar_get_orientation (ExoIconBar *icon_bar)
{
  g_return_val_if_fail (EXO_IS_ICON_BAR (icon_bar), GTK_ORIENTATION_VERTICAL);
  return icon_bar->priv->orientation;
}



/**
 * exo_icon_bar_set_orientation:
 * @icon_bar    : An #ExoIconBar.
 * @orientation : A new #GtkOrientation.
 *
 * Sets whether the @icon_bar should appear horizontally
 * or vertically.
 **/
void
exo_icon_bar_set_orientation (ExoIconBar    *icon_bar,
                              GtkOrientation orientation)
{
  g_return_if_fail (EXO_IS_ICON_BAR (icon_bar));

  if (icon_bar->priv->orientation != orientation)
    {
      icon_bar->priv->orientation = orientation;
      gtk_widget_queue_resize (GTK_WIDGET (icon_bar));
      g_object_notify (G_OBJECT (icon_bar), "orientation");
    }
}



/**
 * exo_icon_bar_get_active:
 * @icon_bar  : An #ExoIconBar.
 *
 * Returns the index of the currently active item, or -1 if there's no
 * active item.
 *
 * Return value: An integer which is the index of the currently active item,
 *               or -1 if there's no active item.
 **/
gint
exo_icon_bar_get_active (ExoIconBar *icon_bar)
{
  g_return_val_if_fail (EXO_IS_ICON_BAR (icon_bar), -1);

  return (icon_bar->priv->active_item != NULL)
        ? icon_bar->priv->active_item->index
        : -1;
}



/**
 * exo_icon_bar_set_active:
 * @icon_bar  : An #ExoIconBar.
 * @index     : An index in the model passed during construction,
 *              or -1 to have no active item.
 *
 * Sets the active item of @icon_bar to be the item at @index.
 **/
void
exo_icon_bar_set_active (ExoIconBar *icon_bar,
                         gint        index)
{
  g_return_if_fail (EXO_IS_ICON_BAR (icon_bar));
  g_return_if_fail (index == -1 || g_list_nth (icon_bar->priv->items, index) != NULL);

  if ((icon_bar->priv->active_item == NULL && index == -1)
      || (icon_bar->priv->active_item != NULL && index == icon_bar->priv->active_item->index))
    return;

  if (G_UNLIKELY (index >= 0))
    icon_bar->priv->active_item = g_list_nth (icon_bar->priv->items, index)->data;
  else
    icon_bar->priv->active_item = NULL;

  g_signal_emit (G_OBJECT (icon_bar), icon_bar_signals[SELECTION_CHANGED], 0);
  g_object_notify (G_OBJECT (icon_bar), "active");
  gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
}



/**
 * exo_icon_bar_get_active_iter:
 * @icon_bar  : An #ExoIconBar.
 * @iter      : An uninitialized #GtkTreeIter.
 *
 * Sets @iter to point to the current active item, if it exists.
 *
 * Return value: %TRUE if @iter was set.
 **/
gboolean
exo_icon_bar_get_active_iter (ExoIconBar  *icon_bar,
                              GtkTreeIter *iter)
{
  ExoIconBarItem *item;
  GtkTreePath    *path;

  g_return_val_if_fail (EXO_IS_ICON_BAR (icon_bar), FALSE);
  g_return_val_if_fail (iter != NULL, FALSE);

  item = icon_bar->priv->active_item;
  if (item == NULL)
    return FALSE;

  if ((gtk_tree_model_get_flags (icon_bar->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST) == 0)
    {
      path = gtk_tree_path_new_from_indices (item->index, -1);
      gtk_tree_model_get_iter (icon_bar->priv->model, iter, path);
      gtk_tree_path_free (path);
    }
  else
    {
      *iter = item->iter;
    }

  return TRUE;
}



/**
 * exo_icon_bar_set_active_iter:
 * @icon_bar  : An #ExoIconBar.
 * @iter      : The #GtkTreeIter.
 *
 * Sets the current active item to be the one referenced by @iter. @iter
 * must correspond to a path of depth one.
 *
 * This can only be called if @icon_bar is associated with #GtkTreeModel.
 **/
void
exo_icon_bar_set_active_iter (ExoIconBar  *icon_bar,
                              GtkTreeIter *iter)
{
  GtkTreePath *path;

  g_return_if_fail (EXO_IS_ICON_BAR (icon_bar));
  g_return_if_fail (icon_bar->priv->model != NULL);
  g_return_if_fail (iter != NULL);

  path = gtk_tree_model_get_path (icon_bar->priv->model, iter);
  if (G_LIKELY (path != NULL))
    {
      exo_icon_bar_set_active (icon_bar, gtk_tree_path_get_indices (path)[0]);
      gtk_tree_path_free (path);
    }
}



#define __EXO_ICON_BAR_C__
#include <exo/exo-aliasdef.c>
