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

#include <exo/exo-cell-renderer-ellipsized-text.h>
#include <exo/exo-config.h>
#include <exo/exo-enum-types.h>
#include <exo/exo-pango-extensions.h>
#include <exo/exo-string.h>
#include <exo/exo-alias.h>



GType
exo_cell_renderer_ellipsized_text_get_type (void)
{
  GType type;

  /* check if ExoCellRendererEllipsizedText is already registered */
  type = g_type_from_name ("ExoCellRendererEllipsizedText");
  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      const GTypeInfo info =
      {
        sizeof (ExoCellRendererEllipsizedTextClass),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        sizeof (ExoCellRendererEllipsizedText),
        0,
        NULL,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_CELL_RENDERER_TEXT, "ExoCellRendererEllipsizedText", &info, 0);
    }

  return type;
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
 *
 * Deprecated: 0.3.1.8: Use #GtkCellRendererText instead.
 **/
GtkCellRenderer*
exo_cell_renderer_ellipsized_text_new (void)
{
  return g_object_new (EXO_TYPE_CELL_RENDERER_ELLIPSIZED_TEXT, NULL);
}



#define __EXO_CELL_RENDERER_ELLIPSIZED_TEXT_C__
#include <exo/exo-aliasdef.c>
