/* $Id$ */
/*-
 * Copyright (c) 2004 Benedikt Meurer <benny@xfce.org>
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

#include <exo/exo-ellipsized-label.h>
#include <exo/exo-enum-types.h>
#include <exo/exo-string.h>



#define EXO_ELLIPSIZED_LABEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_ELLIPSIZED_LABEL, ExoEllipsizedLabelPrivate))



enum
{
  PROP_0,
  PROP_ELLIPSIZE,
};



static void     exo_ellipsized_label_class_init    (ExoEllipsizedLabelClass *klass);
static void     exo_ellipsized_label_init          (ExoEllipsizedLabel      *label);
static void     exo_ellipsized_label_get_property  (GObject                 *object,
                                                    guint                    prop_id,
                                                    GValue                  *value,
                                                    GParamSpec              *pspec);
static void     exo_ellipsized_label_set_property  (GObject                 *object,
                                                    guint                    prop_id,
                                                    const GValue            *value,
                                                    GParamSpec              *pspec);
static gboolean exo_ellipsized_label_expose_event  (GtkWidget               *widget,
                                                    GdkEventExpose          *event);
static void     exo_ellipsized_label_size_request  (GtkWidget               *widget,
                                                    GtkRequisition          *requisition);
static void     exo_ellipsized_label_size_allocate (GtkWidget               *widget,
                                                    GtkAllocation           *allocation);



struct _ExoEllipsizedLabelPrivate
{
  ExoPangoEllipsizeMode ellipsize;
};



static GObjectClass *parent_class;



G_DEFINE_TYPE (ExoEllipsizedLabel, exo_ellipsized_label, GTK_TYPE_LABEL);



static void
exo_ellipsized_label_class_init (ExoEllipsizedLabelClass *klass)
{
  GtkWidgetClass  *gtkwidget_class;
  GObjectClass    *gobject_class;

  /* GtkLabel has the "ellipsize" property build-in with
   * Gtk+ 2.5 and above.
   */
  if (gtk_major_version == 2 && gtk_minor_version <= 4)
    {
      g_type_class_add_private (klass, sizeof (ExoEllipsizedLabelPrivate));

      parent_class = g_type_class_peek_parent (klass);

      gobject_class                   = G_OBJECT_CLASS (klass);
      gobject_class->get_property     = exo_ellipsized_label_get_property;
      gobject_class->set_property     = exo_ellipsized_label_set_property;

      gtkwidget_class                 = GTK_WIDGET_CLASS (klass);
      gtkwidget_class->expose_event   = exo_ellipsized_label_expose_event;
      gtkwidget_class->size_request   = exo_ellipsized_label_size_request;
      gtkwidget_class->size_allocate  = exo_ellipsized_label_size_allocate;

      /**
       * ExoEllipsizedLabel:ellipsize:
       *
       * The preferred place to ellipsize the string, if the label does not have 
       * enough room to display the entire string, specified as a #ExoPangoEllisizeMode. 
       *
       * Note that setting this property to a value other than %EXO_PANGO_ELLIPSIZE_NONE 
       * has the side-effect that the label requests only enough space to display the
       * ellipsis "...". Ellipsizing labels must be packed in a container which 
       * ensures that the label gets a reasonable size allocated. In particular, 
       * this means that ellipsizing labels don't work well in notebook tabs, unless
       * the tab's ::tab-expand property is set to %TRUE.
       */
      g_object_class_install_property (gobject_class,
                                       PROP_ELLIPSIZE,
                                       g_param_spec_enum ("ellipsize",
                                                          _("Ellipsize"),
                                                          _("The preferred place to ellipsize the string, "
                                                            "if the label does not have enough room to "
                                                            "display the entire string, if at all"),
                                                          EXO_TYPE_PANGO_ELLIPSIZE_MODE,
                                                          EXO_PANGO_ELLIPSIZE_NONE,
                                                          G_PARAM_READWRITE));
    }
}



static void
exo_ellipsized_label_init (ExoEllipsizedLabel *label)
{
  if (gtk_major_version == 2 && gtk_minor_version <= 4)
    {
      label->priv            = EXO_ELLIPSIZED_LABEL_GET_PRIVATE (label);
      label->priv->ellipsize = EXO_PANGO_ELLIPSIZE_NONE;
    }
}



static void
exo_ellipsized_label_get_property (GObject     *object,
                                   guint        prop_id,
                                   GValue      *value,
                                   GParamSpec  *pspec)
{
  ExoEllipsizedLabel *label = EXO_ELLIPSIZED_LABEL (object);

  switch (prop_id)
    {
    case PROP_ELLIPSIZE:
      g_value_set_enum (value, label->priv->ellipsize);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_ellipsized_label_set_property (GObject       *object,
                                   guint          prop_id,
                                   const GValue  *value,
                                   GParamSpec    *pspec)
{
  ExoEllipsizedLabel *label = EXO_ELLIPSIZED_LABEL (object);

  switch (prop_id)
    {
    case PROP_ELLIPSIZE:
      exo_ellipsized_label_set_ellipsize (label, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gboolean
exo_ellipsized_label_expose_event (GtkWidget       *widget,
                                   GdkEventExpose  *event)
{
  GtkRequisition req;

  GTK_WIDGET_CLASS (parent_class)->size_request (widget, &req);
  widget->requisition.width = req.width;
  GTK_WIDGET_CLASS (parent_class)->expose_event (widget, event);
  widget->requisition.width = 0;

  return FALSE;
}



static void
exo_ellipsized_label_size_request (GtkWidget      *widget,
                                   GtkRequisition *requisition)
{
  PangoFontMetrics *metrics;
  PangoContext     *context;
  gint              char_width;

  GTK_WIDGET_CLASS (parent_class)->size_request (widget, requisition);

  /* determine the width for 3 characters ("...") */
  context = pango_layout_get_context (GTK_LABEL (widget)->layout);
  metrics = pango_context_get_metrics (context, widget->style->font_desc, NULL);
  char_width = pango_font_metrics_get_approximate_char_width (metrics);
  pango_font_metrics_unref (metrics);

  requisition->width = PANGO_PIXELS (char_width) * 3;
}



static void
exo_ellipsized_label_size_allocate (GtkWidget      *widget,
                                     GtkAllocation *allocation)
{
  ExoEllipsizedLabel *label = EXO_ELLIPSIZED_LABEL (widget);

  if (G_LIKELY (GTK_LABEL (label)->layout != NULL))
    {
      if (G_UNLIKELY (GTK_LABEL (label)->text == NULL))
        {
          pango_layout_set_text (GTK_LABEL (label)->layout, "", -1);
        }
      else
        {
          exo_pango_layout_set_text_ellipsized (GTK_LABEL (label)->layout,
                                                GTK_LABEL (label)->text,
                                                allocation->width,
                                                label->priv->ellipsize);
        }
    }

  GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);
}



/**
 * exo_ellipsized_label_new:
 * @text  : The text of the label.
 *
 * Creates a new #ExoEllipsizedLabel with the given
 * text inside it. You can pass %NULL to get an
 * empty label widget.
 *
 * Return value: The new #ExoEllipsizedLabel.
 **/
GtkWidget*
exo_ellipsized_label_new (const gchar *text)
{
  return g_object_new (EXO_TYPE_ELLIPSIZED_LABEL,
                       "label", text,
                       NULL);
}



/**
 * exo_ellipsized_label_get_ellipsize:
 * @label : An #ExoEllipsizedLabel.
 *
 * Returns the ellipsizing position of the @label.
 * See exo_ellipsized_label_set_ellipsize().
 *
 * Return value: An #ExoPangoEllipsizeMode.
 **/
ExoPangoEllipsizeMode
exo_ellipsized_label_get_ellipsize (ExoEllipsizedLabel *label)
{
  ExoPangoEllipsizeMode ellipsize;

  g_return_val_if_fail (EXO_IS_ELLIPSIZED_LABEL (label),
                        EXO_PANGO_ELLIPSIZE_NONE);

  if (gtk_major_version == 2 && gtk_minor_version <= 4)
    ellipsize = label->priv->ellipsize;
  else
    g_object_get (G_OBJECT (label), "ellipsize", &ellipsize, NULL);

  return ellipsize;
}



/**
 * exo_ellipsized_label_set_ellipsize:
 * @label     : An #ExoEllipsizedLabel.
 * @ellipsize : An #ExoPangoEllipsizeMode.
 *
 * Sets the mode used to ellipsize (add an ellipsis: "...") to the
 * text if there is not enough space to render the entire string.
 **/
void
exo_ellipsized_label_set_ellipsize (ExoEllipsizedLabel   *label,
                                    ExoPangoEllipsizeMode ellipsize)
{
  g_return_if_fail (EXO_IS_ELLIPSIZED_LABEL (label));

  if (gtk_major_version == 2 && gtk_minor_version <= 4)
    {
      if (ellipsize != label->priv->ellipsize)
        {
          label->priv->ellipsize = ellipsize;
          gtk_widget_queue_resize (GTK_WIDGET (label));
          g_object_notify (G_OBJECT (label), "ellipsize");
        }
    }
  else
    {
      g_object_set (G_OBJECT (label), "ellipsize", ellipsize, NULL);
    }
}

