/* $Id$ */
/*-
 * Copyright (c) 2000      Ramiro Estrugo <ramiro@eazel.com>
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>
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
#include <exo/exo-wrap-table.h>
#include <exo/exo-alias.h>



#define EXO_WRAP_TABLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_WRAP_TABLE, ExoWrapTablePrivate))



/* Property identifiers */
enum
{
  PROP_0,
  PROP_COL_SPACING,
  PROP_ROW_SPACING,
  PROP_HOMOGENEOUS,
};



static void exo_wrap_table_class_init         (ExoWrapTableClass  *klass);
static void exo_wrap_table_init               (ExoWrapTable       *table);
static void exo_wrap_table_get_property       (GObject            *object,
                                               guint               prop_id,
                                               GValue             *value,
                                               GParamSpec         *pspec);
static void exo_wrap_table_set_property       (GObject            *object,
                                               guint               prop_id,
                                               const GValue       *value,
                                               GParamSpec         *pspec);
static void exo_wrap_table_size_request       (GtkWidget          *widget,
                                               GtkRequisition     *requisition);
static void exo_wrap_table_size_allocate      (GtkWidget          *widget,
                                               GtkAllocation      *allocation);
static void exo_wrap_table_add                (GtkContainer       *container,
                                               GtkWidget          *widget);
static void exo_wrap_table_remove             (GtkContainer       *container,
                                               GtkWidget          *widget);
static void exo_wrap_table_forall             (GtkContainer       *container,
                                               gboolean            include_internals,
                                               GtkCallback         callback,
                                               gpointer            callback_data);
static void exo_wrap_table_layout             (ExoWrapTable       *table);
static gint exo_wrap_table_get_max_child_size (const ExoWrapTable *table,
                                               gint               *max_width_return,
                                               gint               *max_height_return);
static gint exo_wrap_table_get_num_fitting    (gint                available,
                                               gint                spacing,
                                               gint                max_child_size);



struct _ExoWrapTablePrivate
{
  /* the list of child widgets */
  GList *children;

  /* configurable parameters */
  guint  col_spacing;
  guint  row_spacing;
  guint  homogeneous : 1;

  /* the estimated number of columns */
  gint   num_cols;
};



static GObjectClass *exo_wrap_table_parent_class;



GType
exo_wrap_table_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = _exo_g_type_register_simple (GTK_TYPE_CONTAINER,
                                          "ExoWrapTable",
                                          sizeof (ExoWrapTableClass),
                                          exo_wrap_table_class_init,
                                          sizeof (ExoWrapTable),
                                          exo_wrap_table_init);
    }

  return type;
}



static void
exo_wrap_table_class_init (ExoWrapTableClass *klass)
{
  GtkContainerClass *gtkcontainer_class;
  GtkWidgetClass    *gtkwidget_class;
  GObjectClass      *gobject_class;

  /* add our private data to the class */
  g_type_class_add_private (klass, sizeof (ExoWrapTablePrivate));

  /* determine our parent type class */
  exo_wrap_table_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->get_property = exo_wrap_table_get_property;
  gobject_class->set_property = exo_wrap_table_set_property;

  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->size_request = exo_wrap_table_size_request;
  gtkwidget_class->size_allocate = exo_wrap_table_size_allocate;

  gtkcontainer_class = GTK_CONTAINER_CLASS (klass);
  gtkcontainer_class->add = exo_wrap_table_add;
  gtkcontainer_class->remove = exo_wrap_table_remove;
  gtkcontainer_class->forall = exo_wrap_table_forall;

  /* initialize the library's i18n support */
  _exo_i18n_init ();

  /**
   * ExoWrapTable::col-spacing:
   *
   * The amount of space between two consecutive columns.
   *
   * Since: 0.3.1
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COL_SPACING,
                                   g_param_spec_uint ("col-spacing",
                                                      _("Column spacing"),
                                                      _("The amount of space between two consecutive columns"),
                                                      0, G_MAXUINT, 0,
                                                      EXO_PARAM_READWRITE));

  /**
   * ExoWrapTable::row-spacing:
   *
   * The amount of space between two consecutive rows.
   *
   * Since: 0.3.1
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ROW_SPACING,
                                   g_param_spec_uint ("row-spacing",
                                                      _("Row spacing"),
                                                      _("The amount of space between two consecutive rows"),
                                                      0, G_MAXUINT, 0,
                                                      EXO_PARAM_READWRITE));

  /**
   * ExoWrapTable::homogeneous:
   *
   * Whether the children should be all the same size.
   *
   * Since: 0.3.1
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_HOMOGENEOUS,
                                   g_param_spec_boolean ("homogeneous",
                                                         _("Homogeneous"),
                                                         _("Whether the children should be all the same size"),
                                                         FALSE,
                                                         EXO_PARAM_READWRITE));
}



static void
exo_wrap_table_init (ExoWrapTable *table)
{
  /* grab a pointer on the private data */
  table->priv = EXO_WRAP_TABLE_GET_PRIVATE (table);

  /* we don't provide our own window */
  GTK_WIDGET_SET_FLAGS (table, GTK_NO_WINDOW);
}



static void
exo_wrap_table_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  ExoWrapTable *table = EXO_WRAP_TABLE (object);

  switch (prop_id)
    {
    case PROP_COL_SPACING:
      g_value_set_uint (value, exo_wrap_table_get_col_spacing (table));
      break;

    case PROP_ROW_SPACING:
      g_value_set_uint (value, exo_wrap_table_get_row_spacing (table));
      break;

    case PROP_HOMOGENEOUS:
      g_value_set_boolean (value, exo_wrap_table_get_homogeneous (table));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_wrap_table_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  ExoWrapTable *table = EXO_WRAP_TABLE (object);

  switch (prop_id)
    {
    case PROP_COL_SPACING:
      exo_wrap_table_set_col_spacing (table, g_value_get_uint (value));
      break;

    case PROP_ROW_SPACING:
      exo_wrap_table_set_row_spacing (table, g_value_get_uint (value));
      break;

    case PROP_HOMOGENEOUS:
      exo_wrap_table_set_homogeneous (table, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_wrap_table_size_request (GtkWidget      *widget,
                             GtkRequisition *requisition)
{
  ExoWrapTable *table = EXO_WRAP_TABLE (widget);
  gint          max_width = 0;
  gint          max_height = 0;
  gint          num_children;
  gint          num_cols;
  gint          num_rows;

  /* determine the max size request */
  num_children = exo_wrap_table_get_max_child_size (table, &max_width, &max_height);

  /* check if we have any visible children */
  if (G_LIKELY (num_children > 0))
    {
      num_cols = exo_wrap_table_get_num_fitting (widget->allocation.width
                                                 - GTK_CONTAINER (widget)->border_width * 2,
                                                 table->priv->col_spacing, max_width);
      num_rows = num_children / num_cols;
      num_rows = MAX (num_rows, 1);

      if ((num_children % num_rows) > 0)
        ++num_rows;

      requisition->width = -1;
      requisition->height = (num_rows * max_height)
                         + (num_rows - 1) * table->priv->col_spacing
                         + GTK_CONTAINER (widget)->border_width * 2;
    }
  else
    {
      requisition->width = 0;
      requisition->height = 0;
    }
}



static void
exo_wrap_table_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
  ExoWrapTable *table = EXO_WRAP_TABLE (widget);

  /* setup the new allocation */
  widget->allocation = *allocation;

  /* layout the children */
  exo_wrap_table_layout (table);
}



static void
exo_wrap_table_add (GtkContainer *container,
                    GtkWidget    *widget)
{
  ExoWrapTable *table = EXO_WRAP_TABLE (container);

  /* take over ownership */
  gtk_widget_set_parent (widget, GTK_WIDGET (table));

  /* add the child to our internal list */
  table->priv->children = g_list_append (table->priv->children, widget);

  /* realize the widget if required */
  if (GTK_WIDGET_REALIZED (container))
    gtk_widget_realize (widget);

  /* map the widget if required */
  if (GTK_WIDGET_VISIBLE (container) && GTK_WIDGET_VISIBLE (widget))
    {
      if (GTK_WIDGET_MAPPED (container))
        gtk_widget_map (widget);
    }

  /* queue a resize on the table */
  gtk_widget_queue_resize (GTK_WIDGET (container));
}



static void
exo_wrap_table_remove (GtkContainer *container,
                       GtkWidget    *widget)
{
  ExoWrapTable *table = EXO_WRAP_TABLE (container);
  gboolean      widget_was_visible;

  /* check if the widget was visible */
  widget_was_visible = GTK_WIDGET_VISIBLE (widget);

  /* unparent and remove the widget */
  gtk_widget_unparent (widget);
  table->priv->children = g_list_remove (table->priv->children, widget);

  /* schedule a resize if the widget was visible */
  if (G_LIKELY (widget_was_visible))
    gtk_widget_queue_resize (GTK_WIDGET (table));
}



static void
exo_wrap_table_forall (GtkContainer *container,
                       gboolean      include_internals,
                       GtkCallback   callback,
                       gpointer      callback_data)
{
  ExoWrapTable *table = EXO_WRAP_TABLE (container);
  GList        *next;
  GList        *node;

  for (node = table->priv->children; node != NULL; node = next)
    {
      /* verify that we have a valid widget for the node */
      g_assert (GTK_IS_WIDGET (node->data));

      /* remember a pointer to the next node */
      next = node->next;

      /* invoke the callback for this widget */
      (*callback) (GTK_WIDGET (node->data), callback_data);
    }
}



static void
exo_wrap_table_layout (ExoWrapTable *table)
{
  GtkRequisition child_requisition;
  GtkAllocation  child_allocation;
  GtkWidget     *child;
  GList         *lp;
  gint           x0, x1, x, y;
  gint           num_children;
  gint           num_cols;
  gint           max_height;
  gint           max_width;

  /* determine the number of visible children and the max size */
  num_children = exo_wrap_table_get_max_child_size (table, &max_width, &max_height);
  if (G_UNLIKELY (num_children <= 0))
    return;

  /* determine the number of columns */
  num_cols = exo_wrap_table_get_num_fitting (GTK_WIDGET (table)->allocation.width
                                             - GTK_CONTAINER (table)->border_width * 2,
                                             table->priv->col_spacing, max_width);

  /* verify that the number of columns match */
  if (G_UNLIKELY (num_cols != table->priv->num_cols))
    {
      table->priv->num_cols = num_cols;
      gtk_widget_queue_resize (GTK_WIDGET (table));
      return;
    }

  /* determine the horizontal bounds */
  x0 = GTK_WIDGET (table)->allocation.x + GTK_CONTAINER (table)->border_width;
  x1 = x0 + GTK_WIDGET (table)->allocation.width - GTK_CONTAINER (table)->border_width;

  /* initialize the position */
  x = x0;
  y = GTK_WIDGET (table)->allocation.y + GTK_CONTAINER (table)->border_width;

  /* allocate space to all visible children */
  for (lp = table->priv->children; lp != NULL; lp = lp->next)
    {
      /* allocate space only for visible children */
      child = GTK_WIDGET (lp->data);
      if (G_UNLIKELY (!GTK_WIDGET_VISIBLE (child)))
        continue;

      /* initialize the child position */
      child_allocation.x = x;
      child_allocation.y = y;

      /* check if we should layout the children homogeneously */
      if (G_LIKELY (table->priv->homogeneous))
        {
          child_allocation.width = max_width;
          child_allocation.height = max_height;

          /* check if we're wrapping */
          if (G_UNLIKELY ((x + max_width) > x1))
            {
              x = x0 + table->priv->col_spacing + max_width;
              y += table->priv->row_spacing + max_height;
              child_allocation.x = x0;
              child_allocation.y = y;
            }
          else
            {
              x += table->priv->col_spacing + max_width;
            }
        }
      else
        {
          gtk_widget_size_request (child, &child_requisition);

          child_allocation.width = child_requisition.width;
          child_allocation.height = child_requisition.height;

          g_assert (child_allocation.width <= max_width);
          g_assert (child_allocation.height <= max_height);

          if (G_UNLIKELY ((x + max_width) > x1))
            {
              x = x0 + table->priv->col_spacing + max_width;
              y += table->priv->row_spacing + max_height;
              child_allocation.x = x0;
              child_allocation.y = y;
            }
          else
            {
              x += table->priv->col_spacing + max_width;
            }
        }

      /* allocate the space to the child */
      gtk_widget_size_allocate (child, &child_allocation);
    }
}



static gint
exo_wrap_table_get_max_child_size (const ExoWrapTable *table,
                                   gint               *max_width_return,
                                   gint               *max_height_return)
{
  GtkRequisition child_requisition;
  GtkWidget     *child;
  GList         *lp;
  gint           max_width = 0;
  gint           max_height = 0;
  gint           num_children = 0;

  for (lp = table->priv->children; lp != NULL; lp = lp->next)
    {
      child = GTK_WIDGET (lp->data);
      if (GTK_WIDGET_VISIBLE (child))
        {
          gtk_widget_size_request (child, &child_requisition);
          if (child_requisition.width > max_width)
            max_width = child_requisition.width;
          if (child_requisition.height > max_height)
            max_height = child_requisition.height;

          /* we count only visible children */
          ++num_children;
        }
    }

  /* use atleast one pixel if we have visible childrens */
  if (G_LIKELY (num_children > 0))
    {
      if (G_UNLIKELY (max_width < 1))
        max_width = 1;
      if (G_UNLIKELY (max_height < 1))
        max_height = 1;
    }

  /* return the determined values */
  if (G_LIKELY (max_width_return != NULL))
    *max_width_return = max_width;
  if (G_LIKELY (max_height_return != NULL))
    *max_height_return = max_height;

  return num_children;
}



static gint
exo_wrap_table_get_num_fitting (gint available,
                                gint spacing,
                                gint max_child_size)
{
  gint num;

  g_return_val_if_fail (spacing >= 0, 0);
  g_return_val_if_fail (max_child_size > 0, 0);

  /* verify that available is atleast 0 */
  if (G_UNLIKELY (available < 0))
    available = 0;

  /* determine the num */
  num = (available + spacing) / (max_child_size + spacing);

  /* verify that num is atleast 1 */
  if (G_UNLIKELY (num < 1))
    num = 1;

  return num;
}



/**
 * exo_wrap_table_new:
 * @homogeneous : %TRUE if all children are to be given equal space allotments.
 *
 * Allocates a new #ExoWrapTable.
 *
 * Return value: the newly allocated #ExoWrapTable.
 *
 * Since: 0.3.1
 **/
GtkWidget*
exo_wrap_table_new (gboolean homogeneous)
{
  return g_object_new (EXO_TYPE_WRAP_TABLE,
                       "homogeneous", homogeneous,
                       NULL);
}



/**
 * exo_wrap_table_get_col_spacing:
 * @table : an #ExoWrapTable.
 *
 * Returns the amount of space between consecutive
 * columns in @table.
 *
 * Return value: the amount of space between
 *               consecutive columns.
 *
 * Since: 0.3.1
 **/
guint
exo_wrap_table_get_col_spacing (const ExoWrapTable *table)
{
  g_return_val_if_fail (EXO_IS_WRAP_TABLE (table), 0);
  return table->priv->col_spacing;
}



/**
 * exo_wrap_table_set_col_spacing:
 * @table       : an #ExoWrapTable.
 * @col_spacing : the new column spacing.
 *
 * Sets the amount of space between consecutive
 * columns in @table to @col_spacing.
 *
 * Since: 0.3.1
 **/
void
exo_wrap_table_set_col_spacing (ExoWrapTable *table,
                                guint         col_spacing)
{
  g_return_if_fail (EXO_IS_WRAP_TABLE (table));
  
  if (G_LIKELY (table->priv->col_spacing != col_spacing))
    {
      table->priv->col_spacing = col_spacing;
      gtk_widget_queue_resize (GTK_WIDGET (table));
      g_object_notify (G_OBJECT (table), "col-spacing");
    }
}



/**
 * exo_wrap_table_get_row_spacing:
 * @table : an #ExoWrapTable.
 *
 * Returns the amount of space between consecutive
 * rows in @table.
 *
 * Return value: the amount of space between
 *               consecutive rows in @table.
 *
 * Since: 0.3.1
 **/
guint
exo_wrap_table_get_row_spacing (const ExoWrapTable *table)
{
  g_return_val_if_fail (EXO_IS_WRAP_TABLE (table), 0);
  return table->priv->row_spacing;
}



/**
 * exo_wrap_table_set_row_spacing:
 * @table       : an #ExoWrapTable.
 * @row_spacing : the new row spacing.
 *
 * Sets the amount of spacing between consecutive
 * rows in @table to @row_spacing.
 *
 * Since: 0.3.1
 **/
void
exo_wrap_table_set_row_spacing (ExoWrapTable *table,
                                guint         row_spacing)
{
  g_return_if_fail (EXO_IS_WRAP_TABLE (table));

  if (G_LIKELY (table->priv->row_spacing != row_spacing))
    {
      table->priv->row_spacing = row_spacing;
      gtk_widget_queue_resize (GTK_WIDGET (table));
      g_object_notify (G_OBJECT (table), "row-spacing");
    }
}



/**
 * exo_wrap_table_get_homogeneous:
 * @table : an #ExoWrapTable.
 *
 * Returns whether the table cells are all constrained
 * to the same width and height.
 *
 * Return value: %TRUE if the cells are all constrained
 *               to the same size.
 *
 * Since: 0.3.1
 **/
gboolean
exo_wrap_table_get_homogeneous (const ExoWrapTable *table)
{
  g_return_val_if_fail (EXO_IS_WRAP_TABLE (table), FALSE);
  return table->priv->homogeneous;
}



/**
 * exo_wrap_table_set_homogeneous:
 * @table       : an #ExoWrapTable.
 * @homogeneous : Set to %TRUE to ensure all @table cells are the same size.
 *                Set to %FALSE if this is not your desired behaviour.
 *
 * Changes the homogenous property of @table cells, ie. whether all cells
 * are an equal size or not.
 *
 * Since: 0.3.1
 **/
void
exo_wrap_table_set_homogeneous (ExoWrapTable *table,
                                gboolean      homogeneous)
{
  g_return_if_fail (EXO_IS_WRAP_TABLE (table));

  if (G_LIKELY (table->priv->homogeneous != homogeneous))
    {
      table->priv->homogeneous = homogeneous;
      gtk_widget_queue_resize (GTK_WIDGET (table));
      g_object_notify (G_OBJECT (table), "homogeneous");
    }
}



#define __EXO_WRAP_TABLE_C__
#include <exo/exo-aliasdef.c>
