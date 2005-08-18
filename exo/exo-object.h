/* $Id$ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>.
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

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EXO_OBJECT_H__
#define __EXO_OBJECT_H__

#include <glib-object.h>

#include <exo/exo-config.h>

G_BEGIN_DECLS;

typedef struct _ExoObjectClass ExoObjectClass;
typedef struct _ExoObject      ExoObject;

#define EXO_TYPE_OBJECT            (exo_object_get_type ())
#define EXO_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_OBJECT, ExoObject))
#define EXO_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_OBJECT, ExoObjectClass))
#define EXO_IS_OBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_OBJECT))
#define EXO_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_OBJECT))
#define EXO_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_OBJECT, ExoObjectClass))

struct _ExoObjectClass
{
  GTypeClass __parent__;

  /* Called when the last reference on the object is dropped. */
  void (*finalize) (ExoObject *object);

  /*< private >*/
  void (*reserved1) (void);
  void (*reserved2) (void);
  void (*reserved3) (void);
  void (*reserved4) (void);
  void (*reserved5) (void);
  void (*reserved6) (void);
};

struct _ExoObject
{
  /*< private >*/
  GTypeInstance __parent__;
  gint          ref_count;
  gpointer      reserved1;
  gpointer      reserved2;
};

GType     exo_object_get_type  (void) G_GNUC_CONST;

gpointer  exo_object_new       (GType    type);

gpointer  exo_object_ref       (gpointer object);
void      exo_object_unref     (gpointer object);


typedef struct _ExoParamSpecObject ExoParamSpecObject;

#define EXO_TYPE_PARAM_OBJECT           (exo_param_spec_object_get_type ())
#define EXO_PARAM_SPEC_OBJECT(pspec)    (G_TYPE_CHECK_INSTANCE_CAST ((pspec), EXO_TYPE_PARAM_OBJECT, ExoParamObject))
#define EXO_IS_PARAM_SPEC_OBJECT(pspec) (G_TYPE_CHECK_INSTANCE_TYPE ((pspec), EXO_TYPE_PARAM_OBJECT))
#define EXO_VALUE_HOLDS_OBJECT(value)   (G_TYPE_CHECK_VALUE_TYPE ((value), EXO_TYPE_OBJECT))

struct _ExoParamSpecObject
{
  /*< private >*/
  GParamSpec __parent__;
};

GType       exo_param_spec_object_get_type (void) G_GNUC_CONST;

GParamSpec *exo_param_spec_object          (const gchar   *name,
                                            const gchar   *nick,
                                            const gchar   *blurb,
                                            GType          object_type,
                                            GParamFlags    flags);

void        exo_value_set_object           (GValue        *value,
                                            gpointer       object);
void        exo_value_take_object          (GValue        *value,
                                            gpointer       object);
gpointer    exo_value_get_object           (const GValue  *value);
gpointer    exo_value_dup_object           (const GValue  *value);

G_END_DECLS;

#endif /* !__EXO_OBJECT_H__ */
