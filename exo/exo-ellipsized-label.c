/* $Id$ */
/*-
 * Copyright (c) 2004 Benedikt Meurer <benny@xfce.org>
 * Copyright (c) 2000 John Sullivan <sullivan@eazel.com>
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

#include <exo/exo-ellipsized-label.h>
#include <exo/exo-string.h>



#define EXO_ELLIPSIZED_LABEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_ELLIPSIZED_LABEL, ExoEllipsizedLabelPrivate))



enum
{
  PROP_0,
  PROP_FULL_TEXT,
};



static void     exo_ellipsized_label_finalize      (GObject        *object);
static void     exo_ellipsized_label_get_property  (GObject        *object,
                                                    guint           prop_id,
                                                    GValue         *value,
                                                    GParamSpec     *pspec);
static void     exo_ellipsized_label_set_property  (GObject        *object,
                                                    guint           prop_id,
                                                    const GValue   *value,
                                                    GParamSpec     *pspec);
static gboolean exo_ellipsized_label_expose_event  (GtkWidget      *widget,
                                                    GdkEventExpose *event);
static void     exo_ellipsized_label_size_request  (GtkWidget      *widget,
                                                    GtkRequisition *requisition);
static void     exo_ellipsized_label_size_allocate (GtkWidget      *widget,
                                                    GtkAllocation  *allocation);



struct _ExoEllipsizedLabelPrivate
{
  gchar                *full_text;
  GtkTooltips          *tips;
  ExoPangoEllipsizeMode mode;
};



static GObjectClass *parent_class;



G_DEFINE_TYPE (ExoEllipsizedLabel, exo_ellipsized_label, GTK_TYPE_LABEL);



static void
exo_ellipsized_label_class_init (ExoEllipsizedLabelClass *klass)
{
  GtkWidgetClass  *gtkwidget_class;
  GObjectClass    *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoEllipsizedLabelPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class                   = G_OBJECT_CLASS (klass);
  gobject_class->finalize         = exo_ellipsized_label_finalize;
  gobject_class->get_property     = exo_ellipsized_label_get_property;
  gobject_class->set_property     = exo_ellipsized_label_set_property;

  gtkwidget_class                 = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->expose_event   = exo_ellipsized_label_expose_event;
  gtkwidget_class->size_request   = exo_ellipsized_label_size_request;
  gtkwidget_class->size_allocate  = exo_ellipsized_label_size_allocate;

  /**
   * ExoEllipsizedLabel:full-text:
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FULL_TEXT,
                                   g_param_spec_string ("full-text",
                                                        "Full text",
                                                        "Full text, will be ellipsized",
                                                        NULL,
                                                        G_PARAM_WRITABLE));
}



static void
exo_ellipsized_label_init (ExoEllipsizedLabel *label)
{
  label->priv       = EXO_ELLIPSIZED_LABEL_GET_PRIVATE (label);
  label->priv->tips = gtk_tooltips_new ();
  label->priv->mode = EXO_PANGO_ELLIPSIZE_END;

  /* f*ck the floating ref thang */
  g_object_ref (G_OBJECT (label->priv->tips));
  gtk_object_sink (GTK_OBJECT (label->priv->tips));
}



static void
exo_ellipsized_label_finalize (GObject *object)
{
  ExoEllipsizedLabel *label = EXO_ELLIPSIZED_LABEL (object);

  if (G_LIKELY (label->priv->full_text != NULL))
    g_free (label->priv->full_text);
  g_object_unref (label->priv->tips);

  parent_class->finalize (object);
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
    case PROP_FULL_TEXT:
      g_value_set_string (value, exo_ellipsized_label_get_full_text (label));
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
    case PROP_FULL_TEXT:
      exo_ellipsized_label_set_full_text (label, g_value_get_string (value));
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
exo_ellipsized_label_size_request (GtkWidget       *widget,
                                    GtkRequisition *requisition)
{
  GTK_WIDGET_CLASS (parent_class)->size_request (widget, requisition);
  requisition->width = 0;
}



static void
exo_ellipsized_label_size_allocate (GtkWidget      *widget,
                                     GtkAllocation *allocation)
{
  ExoEllipsizedLabel *label = EXO_ELLIPSIZED_LABEL (widget);
  gboolean            show_tips;

  if (G_LIKELY (GTK_LABEL (label)->layout != NULL))
    {
      if (G_UNLIKELY (label->priv->full_text == NULL))
        {
          pango_layout_set_text (GTK_LABEL (label)->layout, "", -1);
          show_tips = FALSE;
        }
      else
        {
          show_tips = exo_pango_layout_set_text_ellipsized (GTK_LABEL (label)->layout,
                                                             label->priv->full_text,
                                                             allocation->width,
                                                             label->priv->mode);
        }

      if (show_tips)
        {
          gtk_tooltips_enable  (label->priv->tips);
          gtk_tooltips_set_tip (label->priv->tips, widget, 
                                label->priv->full_text, NULL);
        }
      else
        {
          gtk_tooltips_disable (label->priv->tips);
        }
    }

  GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);
}



/**
 * exo_ellipsized_label_new:
 * @full_text   :
 *
 * Return value :
 **/
GtkWidget*
exo_ellipsized_label_new (const gchar *full_text)
{
  return g_object_new (EXO_TYPE_ELLIPSIZED_LABEL,
                       "full_text", full_text,
                       NULL);
}



/**
 * exo_ellipsized_label_get_full_text:
 * @label       : A #ExoEllipsizedLabel.
 *
 * Return value :
 **/
const gchar*
exo_ellipsized_label_get_full_text (ExoEllipsizedLabel *label)
{
  g_return_val_if_fail (EXO_IS_ELLIPSIZED_LABEL (label), NULL);

  return label->priv->full_text;
}



/**
 * exo_ellipsized_label_set_full_text:
 * @label       : A #ExoEllipsizedLabel.
 * @full_text   :
 *
 * Return value :
 **/
void
exo_ellipsized_label_set_full_text (ExoEllipsizedLabel *label,
                                    const gchar        *full_text)
{
  g_return_if_fail (EXO_IS_ELLIPSIZED_LABEL (label));

  if (G_UNLIKELY (exo_str_is_equal (label->priv->full_text, full_text)))
    return;

  if (label->priv->full_text != NULL)
    g_free (label->priv->full_text);
  label->priv->full_text = g_strdup (full_text != NULL ? full_text : "");

  /* queues a resize as side effect */
  gtk_label_set_text (GTK_LABEL (label), label->priv->full_text);
}



/**
 * exo_ellipsized_label_get_mode:
 * @label       :
 *
 * Return value :
 **/
ExoPangoEllipsizeMode
exo_ellipsized_label_get_mode (ExoEllipsizedLabel *label)
{
  g_return_val_if_fail (EXO_IS_ELLIPSIZED_LABEL (label),
                        EXO_PANGO_ELLIPSIZE_START);

  return label->priv->mode;
}



/**
 * exo_ellipsized_label_set_mode:
 * @label :
 * @mode  :
 **/
void
exo_ellipsized_label_set_mode (ExoEllipsizedLabel   *label,
                               ExoPangoEllipsizeMode mode)
{
  g_return_if_fail (EXO_IS_ELLIPSIZED_LABEL (label));

  if (mode == label->priv->mode)
    return;

  label->priv->mode = mode;

  /* queues a resize as side effect */
  gtk_label_set_text (GTK_LABEL (label), label->priv->full_text);
}


