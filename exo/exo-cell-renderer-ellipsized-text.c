/* $Id: exo-xsession-client.c 25 2004-11-23 15:07:56Z bmeurer $ */
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

#include <exo/exo-cell-renderer-ellipsized-text.h>
#include <exo/exo-enum-types.h>
#include <exo/exo-pango-extensions.h>



#define EXO_CELL_RENDERER_ELLIPSIZED_TEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_CELL_RENDERER_ELLIPSIZED_TEXT, ExoCellRendererEllipsizedTextPrivate))



enum
{
  PROP_0,
  PROP_ELLIPSIZE,
  PROP_ELLIPSIZE_SET,
};



static void exo_cell_renderer_ellipsized_text_class_init    (ExoCellRendererEllipsizedTextClass *klass);
static void exo_cell_renderer_ellipsized_text_init          (ExoCellRendererEllipsizedText      *renderer);
static void exo_cell_renderer_ellipsized_text_get_property  (GObject                            *object,
                                                             guint                               prop_id,
                                                             GValue                             *value,
                                                             GParamSpec                         *pspec);
static void exo_cell_renderer_ellipsized_text_set_property  (GObject                            *object,
                                                             guint                               prop_id,
                                                             const GValue                       *value,
                                                             GParamSpec                         *pspec);
static void exo_cell_renderer_ellipsized_text_get_size      (GtkCellRenderer                    *cell,
                                                             GtkWidget                          *widget,
                                                             GdkRectangle                       *cell_area,
                                                             gint                               *x_offset,
                                                             gint                               *y_offset,
                                                             gint                               *width,
                                                             gint                               *height);
static void exo_cell_renderer_ellipsized_text_render        (GtkCellRenderer                    *cell,
                                                             GdkWindow                          *window,
                                                             GtkWidget                          *widget,
                                                             GdkRectangle                       *background_area,
                                                             GdkRectangle                       *cell_area,
                                                             GdkRectangle                       *expose_area,
                                                             guint                               flags);



struct _ExoCellRendererEllipsizedTextPrivate
{
  ExoPangoEllipsizeMode ellipsize;
  guint                 ellipsize_set : 1;
};



static GObjectClass *parent_class;



G_DEFINE_TYPE (ExoCellRendererEllipsizedText, exo_cell_renderer_ellipsized_text, GTK_TYPE_CELL_RENDERER_TEXT);



static void
exo_cell_renderer_ellipsized_text_class_init (ExoCellRendererEllipsizedTextClass *klass)
{
  GtkCellRendererClass *gtkcell_renderer_class;
  GObjectClass         *gobject_class;

  /* Gtk+ 2.5 and above already have the ellipsize property
   * in GtkCellRendererText.
   */
  if (gtk_major_version == 2 && gtk_minor_version <= 4)
    {
      g_type_class_add_private (klass, sizeof (ExoCellRendererEllipsizedTextPrivate));

      parent_class = g_type_class_peek_parent (klass);

      gobject_class = G_OBJECT_CLASS (klass);
      gobject_class->get_property = exo_cell_renderer_ellipsized_text_get_property;
      gobject_class->set_property = exo_cell_renderer_ellipsized_text_set_property;

      gtkcell_renderer_class = GTK_CELL_RENDERER_CLASS (klass);
      gtkcell_renderer_class->get_size = exo_cell_renderer_ellipsized_text_get_size;
      gtkcell_renderer_class->render = exo_cell_renderer_ellipsized_text_render;

      g_object_class_install_property (gobject_class,
                                       PROP_ELLIPSIZE,
                                       g_param_spec_enum ("ellipsize",
                                                          _("Ellipsize"),
                                                          _("The preferred place to ellipsize the string, "
                                                            "if the cell renderer does not have enough "
                                                            "room to display the entire string, if at all"),
                                                          EXO_TYPE_PANGO_ELLIPSIZE_MODE,
                                                          EXO_PANGO_ELLIPSIZE_NONE,
                                                          G_PARAM_READWRITE));

      g_object_class_install_property (gobject_class,
                                       PROP_ELLIPSIZE_SET,
                                       g_param_spec_boolean ("ellipsize-set",
                                                             _("Ellipsize set"),
                                                             _("Whether this tag affects the ellipsize mode"),
                                                             FALSE, G_PARAM_READWRITE));
    }
}



static void
exo_cell_renderer_ellipsized_text_init (ExoCellRendererEllipsizedText *renderer)
{
  if (gtk_major_version == 2 && gtk_minor_version <= 4)
    renderer->priv = EXO_CELL_RENDERER_ELLIPSIZED_TEXT_GET_PRIVATE (renderer);
}



static void
exo_cell_renderer_ellipsized_text_get_property (GObject    *object,
                                                guint       prop_id,
                                                GValue     *value,
                                                GParamSpec *pspec)
{
  ExoCellRendererEllipsizedText *renderer = EXO_CELL_RENDERER_ELLIPSIZED_TEXT (object);

  switch (prop_id)
    {
    case PROP_ELLIPSIZE:
      g_value_set_enum (value, renderer->priv->ellipsize);
      break;

    case PROP_ELLIPSIZE_SET:
      g_value_set_boolean (value, renderer->priv->ellipsize_set);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_cell_renderer_ellipsized_text_set_property (GObject      *object,
                                                guint         prop_id,
                                                const GValue *value,
                                                GParamSpec   *pspec)
{
  ExoCellRendererEllipsizedText *renderer = EXO_CELL_RENDERER_ELLIPSIZED_TEXT (object);
  gboolean                       bval;
  gint                           ival;

  switch (prop_id)
    {
    case PROP_ELLIPSIZE:
      ival = g_value_get_enum (value);
      if (ival != renderer->priv->ellipsize)
        {
          renderer->priv->ellipsize = ival;
          g_object_notify (object, "ellipsize");
        }
      break;

    case PROP_ELLIPSIZE_SET:
      bval = g_value_get_boolean (value);
      if (bval != renderer->priv->ellipsize_set)
        {
          renderer->priv->ellipsize_set = bval;
          g_object_notify (object, "ellipsize-set");
        }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
add_attr (PangoAttrList  *attr_list,
          PangoAttribute *attr)
{
  attr->start_index = 0;
  attr->end_index = G_MAXINT;
  
  pango_attr_list_insert (attr_list, attr);
}



static PangoLayout*
get_layout (GtkCellRendererText *celltext,
            GtkWidget           *widget,
            gboolean             will_render,
            GtkCellRendererState flags)
{
  PangoUnderline uline;
  PangoAttrList *attr_list;
  PangoLayout   *layout;
  gboolean       language_set;
  gboolean       single_paragraph;
  gchar         *language;

  g_object_get (G_OBJECT (celltext),
                "language-set", &language_set,
                "single-paragraph-mode", &single_paragraph,
                NULL);
  
  layout = gtk_widget_create_pango_layout (widget, celltext->text);

  if (celltext->extra_attrs)
    attr_list = pango_attr_list_copy (celltext->extra_attrs);
  else
    attr_list = pango_attr_list_new ();

  pango_layout_set_single_paragraph_mode (layout, single_paragraph);

  if (will_render)
    {
      /* Add options that affect appearance but not size */
      
      /* note that background doesn't go here, since it affects
       * background_area not the PangoLayout area
       */
      
      if (celltext->foreground_set)
        {
          PangoColor color;

          color = celltext->foreground;
          
          add_attr (attr_list,
                    pango_attr_foreground_new (color.red, color.green, color.blue));
        }

      if (celltext->strikethrough_set)
        add_attr (attr_list,
                  pango_attr_strikethrough_new (celltext->strikethrough));
    }

  add_attr (attr_list, pango_attr_font_desc_new (celltext->font));

  if (celltext->scale_set &&
      celltext->font_scale != 1.0)
    add_attr (attr_list, pango_attr_scale_new (celltext->font_scale));
  
  if (celltext->underline_set)
    uline = celltext->underline_style;
  else
    uline = PANGO_UNDERLINE_NONE;

  if (language_set)
    {
      g_object_get (G_OBJECT (celltext), "language", &language, NULL);
      add_attr (attr_list, pango_attr_language_new (pango_language_from_string (language)));
      g_free (language);
    }
  
  if ((flags & GTK_CELL_RENDERER_PRELIT) == GTK_CELL_RENDERER_PRELIT)
    {
      switch (uline)
        {
        case PANGO_UNDERLINE_NONE:
          uline = PANGO_UNDERLINE_SINGLE;
          break;

        case PANGO_UNDERLINE_SINGLE:
          uline = PANGO_UNDERLINE_DOUBLE;
          break;

        default:
          break;
        }
    }

  if (uline != PANGO_UNDERLINE_NONE)
    add_attr (attr_list, pango_attr_underline_new (celltext->underline_style));

  if (celltext->rise_set)
    add_attr (attr_list, pango_attr_rise_new (celltext->rise));
  
  pango_layout_set_attributes (layout, attr_list);
  pango_layout_set_width (layout, -1);

  pango_attr_list_unref (attr_list);
  
  return layout;
}



static void
exo_cell_renderer_ellipsized_text_get_size (GtkCellRenderer *cell,
                                            GtkWidget       *widget,
                                            GdkRectangle    *cell_area,
                                            gint            *x_offset,
                                            gint            *y_offset,
                                            gint            *width,
                                            gint            *height)
{
  ExoCellRendererEllipsizedText *renderer = EXO_CELL_RENDERER_ELLIPSIZED_TEXT (cell);
  GtkCellRendererText           *celltext = GTK_CELL_RENDERER_TEXT (cell);
  PangoFontDescription          *font_desc;
  PangoFontMetrics              *metrics;
  PangoRectangle                 rect;
  PangoContext                  *context;
  PangoLayout                   *layout;
  gint                           char_height;
  gint                           char_width;

  if (celltext->calc_fixed_height)
    {
      font_desc = pango_font_description_copy (widget->style->font_desc);
      pango_font_description_merge (font_desc, celltext->font, TRUE);

      if (celltext->scale_set)
  	    pango_font_description_set_size (font_desc, celltext->font_scale * pango_font_description_get_size (font_desc));

      context = gtk_widget_get_pango_context (widget);

      metrics = pango_context_get_metrics (context, font_desc, pango_context_get_language (context));
      char_height = (pango_font_metrics_get_ascent (metrics) + pango_font_metrics_get_descent (metrics));
      pango_font_metrics_unref (metrics);

      pango_font_description_free (font_desc);

      gtk_cell_renderer_set_fixed_size (cell, cell->width, 2 * cell->ypad + celltext->fixed_height_rows * PANGO_PIXELS (char_height));
      
      if (height != NULL)
        {
          *height = cell->height;
          height = NULL;
        }
      celltext->calc_fixed_height = FALSE;
      if (width == NULL)
        return;
    }

  layout = get_layout (celltext, widget, FALSE, 0);
  pango_layout_get_pixel_extents (layout, NULL, &rect);

  if (width != NULL)
    {
      if (renderer->priv->ellipsize_set && renderer->priv->ellipsize != EXO_PANGO_ELLIPSIZE_NONE)
        {
          context = pango_layout_get_context (layout);
          metrics = pango_context_get_metrics (context, widget->style->font_desc, NULL);
          char_width = pango_font_metrics_get_approximate_char_width (metrics);
          pango_font_metrics_unref (metrics);

          *width = cell->xpad * PANGO_PIXELS (char_width) * 3;
        }
      else
        {
          *width = cell->xpad * 2 + rect.width;
        }
    }

  if (height != NULL)
    *height = cell->ypad * 2 + rect.height;

  if (cell_area != NULL)
    {
      if (x_offset != NULL)
        {
          *x_offset = ((gtk_widget_get_direction (widget) == GTK_TEXT_DIR_RTL) ?
                 (1.0 - cell->xalign) : cell->xalign) * (cell_area->width - rect.width - (2 * cell->xpad));
          *x_offset = MAX (*x_offset, 0);
        }
      if (y_offset != NULL)
        {
          *y_offset = cell->yalign * (cell_area->height - rect.height - (2 * cell->ypad));
          *y_offset = MAX (*y_offset, 0);
        }
    }

  g_object_unref (layout);
}



static void
exo_cell_renderer_ellipsized_text_render (GtkCellRenderer   *cell,
                                          GdkWindow         *window,
                                          GtkWidget         *widget,
                                          GdkRectangle      *background_area,
                                          GdkRectangle      *cell_area,
                                          GdkRectangle      *expose_area,
                                          guint              flags)
{
  ExoCellRendererEllipsizedText *renderer = EXO_CELL_RENDERER_ELLIPSIZED_TEXT (cell);
  GtkCellRendererText           *celltext = GTK_CELL_RENDERER_TEXT (cell);
  GtkStateType                   state;
  PangoLayout                   *layout;
  GdkColor                       color;
  GdkGC                         *gc;
  gint                           x_offset;
  gint                           y_offset;

  if ((flags & GTK_CELL_RENDERER_SELECTED) == GTK_CELL_RENDERER_SELECTED)
    {
      if (GTK_WIDGET_HAS_FOCUS (widget))
        state = GTK_STATE_SELECTED;
      else
        state = GTK_STATE_ACTIVE;
    }
  else if ((flags & GTK_CELL_RENDERER_PRELIT) == GTK_CELL_RENDERER_PRELIT
        && GTK_WIDGET_STATE (widget) == GTK_STATE_PRELIGHT)
    {
      state = GTK_STATE_PRELIGHT;
    }
  else
    {
      if (GTK_WIDGET_STATE (widget) == GTK_STATE_INSENSITIVE)
        state = GTK_STATE_INSENSITIVE;
      else
        state = GTK_STATE_NORMAL;
    }

  layout = get_layout (celltext, widget, TRUE, flags);
  gtk_cell_renderer_get_size (cell, widget, cell_area, &x_offset, &y_offset, NULL, NULL);

  if (celltext->background_set && state != GTK_STATE_SELECTED)
    {
      color.red = celltext->background.red;
      color.green = celltext->background.green;
      color.blue = celltext->background.blue;

      gc = gdk_gc_new (window);
      gdk_gc_set_rgb_fg_color (gc, &color);

      if (expose_area != NULL)
        gdk_gc_set_clip_rectangle (gc, expose_area);

      gdk_draw_rectangle (window, gc, TRUE,
                          background_area->x,
                          background_area->y,
                          background_area->width,
                          background_area->height);

      if (expose_area != NULL)
        gdk_gc_set_clip_rectangle (gc, NULL);

      g_object_unref (G_OBJECT (gc));
    }

  if (renderer->priv->ellipsize_set && renderer->priv->ellipsize != EXO_PANGO_ELLIPSIZE_NONE)
    {
      exo_pango_layout_set_text_ellipsized (layout, celltext->text,
                                            cell_area->width - cell->xpad * 2,
                                            renderer->priv->ellipsize);
    }

  gtk_paint_layout (widget->style, window, state, TRUE,
                    expose_area, widget, "cellrenderertext",
                    cell_area->x + x_offset + cell->xpad,
                    cell_area->y + y_offset + cell->ypad,
                    layout);

  g_object_unref (G_OBJECT (layout));
}



/**
 * exo_cell_renderer_ellipsized_text_new:
 *
 * Creates a new #ExoCellRendererEllipsizedText. Adjust how text is
 * drawn using object properties. Object properties can be set globally
 * (with g_object_set()). Also, with #GtkTreeViewColumn, you can bind a
 * property to a value in a #GtkTreeModel. For example, you can bind the
 * "text" property on the cell renderer to a string value in the model,
 * thus rendering a different string in each row of the #GtkTreeView.
 *
 * Return value: The new cell renderer.
 **/
GtkCellRenderer*
exo_cell_renderer_ellipsized_text_new (void)
{
  return g_object_new (EXO_TYPE_CELL_RENDERER_ELLIPSIZED_TEXT, NULL);
}


