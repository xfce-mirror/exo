/*-
 * Copyright (c) 2000 Ramiro Estrugo <ramiro@eazel.com>
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
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

#ifndef __EXO_WRAP_TABLE_H__
#define __EXO_WRAP_TABLE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ExoWrapTablePrivate ExoWrapTablePrivate;
typedef struct _ExoWrapTableClass   ExoWrapTableClass;
typedef struct _ExoWrapTable        ExoWrapTable;

#define EXO_TYPE_WRAP_TABLE             (exo_wrap_table_get_type ())
#define EXO_WRAP_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_WRAP_TABLE, ExoWrapTable))
#define EXO_WRAP_TABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_WRAP_TABLE, ExoWrapTableClass))
#define EXO_IS_WRAP_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_WRAP_TABLE))
#define EXO_IS_WRAP_TABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_WRAP_TABLE))
#define EXO_WRAP_TABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_WRAP_TABLE, ExoWrapTableClass))

struct _ExoWrapTableClass
{
  /*< private >*/
  GtkContainerClass __parent__;

  /* padding for further expansion */
  void (*reserved1) (void);
  void (*reserved2) (void);
  void (*reserved3) (void);
  void (*reserved4) (void);
};

/**
 * ExoWrapTable:
 *
 *  The #ExoWrapTable struct contains only private fields
* and should not be directly accessed.
 **/
struct _ExoWrapTable
{
  /*< private >*/
  GtkContainer         __parent__;
  ExoWrapTablePrivate *priv;
};

GType      exo_wrap_table_get_type        (void) G_GNUC_CONST;

GtkWidget *exo_wrap_table_new             (gboolean            homogeneous) G_GNUC_MALLOC;

guint      exo_wrap_table_get_col_spacing (const ExoWrapTable *table);
void       exo_wrap_table_set_col_spacing (ExoWrapTable       *table,
                                           guint               col_spacing);

guint      exo_wrap_table_get_row_spacing (const ExoWrapTable *table);
void       exo_wrap_table_set_row_spacing (ExoWrapTable       *table,
                                           guint               row_spacing);

gboolean   exo_wrap_table_get_homogeneous (const ExoWrapTable *table);
void       exo_wrap_table_set_homogeneous (ExoWrapTable       *table,
                                           gboolean            homogeneous);

G_END_DECLS

#endif /* !__EXO_WRAP_TABLE_H__ */
