/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
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

#include <exo/exo-ellipsized-label.h>
#include <exo/exo-alias.h>



GType
exo_ellipsized_label_get_type (void)
{
  GType type;

  /* check if ExoEllipsizedLabel is already registered */
  type = g_type_from_name ("ExoEllipsizedLabel");
  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      const GTypeInfo info =
      {
        sizeof (ExoEllipsizedLabelClass),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        sizeof (ExoEllipsizedLabel),
        0,
        NULL,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_LABEL, "ExoEllipsizedLabel", &info, 0);
    }

  return type;
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
 *
 * Deprecated: 0.3.1.8: Use #GtkLabel instead.
 **/
GtkWidget*
exo_ellipsized_label_new (const gchar *text)
{
  return gtk_label_new (text);
}



/**
 * exo_ellipsized_label_get_ellipsize:
 * @label : An #ExoEllipsizedLabel.
 *
 * Returns the ellipsizing position of the @label.
 * See exo_ellipsized_label_set_ellipsize().
 *
 * Return value: An #PangoEllipsizeMode.
 *
 * Deprecated: 0.3.1.8: Use gtk_label_get_ellipsize() instead.
 **/
PangoEllipsizeMode
exo_ellipsized_label_get_ellipsize (ExoEllipsizedLabel *label)
{
  return gtk_label_get_ellipsize (GTK_LABEL (label));
}



/**
 * exo_ellipsized_label_set_ellipsize:
 * @label     : An #ExoEllipsizedLabel.
 * @ellipsize : An #PangoEllipsizeMode.
 *
 * Sets the mode used to ellipsize (add an ellipsis: "...") to the
 * text if there is not enough space to render the entire string.
 *
 * Deprecated: 0.3.1.8: Use gtk_label_set_ellipsize() instead.
 **/
void
exo_ellipsized_label_set_ellipsize (ExoEllipsizedLabel *label,
                                    PangoEllipsizeMode  ellipsize)
{
  gtk_label_set_ellipsize (GTK_LABEL (label), ellipsize);
}



#define __EXO_ELLIPSIZED_LABEL_C__
#include <exo/exo-aliasdef.c>
