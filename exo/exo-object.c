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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gobject/gvaluecollector.h>

#include <exo/exo-object.h>
#include <exo/exo-noop.h>
#include <exo/exo-alias.h>



static void     exo_object_register_type (GType          *type);
static void     exo_object_class_init    (ExoObjectClass *klass);
static void     exo_object_init          (ExoObject      *object);
static void     exo_object_value_free    (GValue         *value);
static void     exo_object_value_copy    (const GValue   *src_value,
                                          GValue         *dst_value);
static gchar*   exo_object_value_collect (GValue         *value,
                                          guint           n_collect_values,
                                          GTypeCValue    *collect_values,
                                          guint           collect_flags);
static gchar*   exo_object_value_lcopy   (const GValue   *value,
                                          guint           n_collect_values,
                                          GTypeCValue    *collect_values,
                                          guint           collect_flags);



GType
exo_object_get_type (void)
{
  static GType type = G_TYPE_INVALID;
  static GOnce once = G_ONCE_INIT;

  /* thread-safe type registration */
  g_once (&once, (GThreadFunc) exo_object_register_type, &type);

  return type;
}



static void
exo_object_register_type (GType *type)
{
  static const GTypeFundamentalInfo finfo =
  {
    G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE,
  };

  static const GTypeValueTable value_table =
  {
    (gpointer) exo_noop,         /* init is a noop because the value is already zero-filled */
    exo_object_value_free,
    exo_object_value_copy,
    (gpointer) exo_value_get_object,
    "p",
    exo_object_value_collect,
    "p",
    exo_object_value_lcopy,
  };

  static const GTypeInfo info =
  {
    sizeof (ExoObjectClass),
    NULL,
    NULL,
    (GClassInitFunc) exo_object_class_init,
    NULL,
    NULL,
    sizeof (ExoObject),
    0,
    (GInstanceInitFunc) exo_object_init,
    &value_table,
  };

  *type = g_type_register_fundamental (g_type_fundamental_next (), "ExoObject", &info, &finfo, G_TYPE_FLAG_ABSTRACT);
}



static void
exo_object_class_init (ExoObjectClass *klass)
{
  /* we provide a function here so derived classes
   * don't need to test the parent's finalize for
   * NULL prior to calling it.
   */
  klass->finalize = (gpointer) exo_noop;
}



static void
exo_object_init (ExoObject *object)
{
  object->ref_count = 1;
}



static void
exo_object_value_free (GValue *value)
{
  if (G_LIKELY (value->data[0].v_pointer != NULL))
    exo_object_unref (value->data[0].v_pointer);
}



static void
exo_object_value_copy (const GValue *src_value,
                       GValue       *dst_value)
{
  if (G_LIKELY (src_value->data[0].v_pointer != NULL))
    dst_value->data[0].v_pointer = exo_object_ref (src_value->data[0].v_pointer);
}



static gchar*
exo_object_value_collect (GValue      *value,
                          guint        n_collect_values,
                          GTypeCValue *collect_values,
                          guint        collect_flags)
{
  if (G_LIKELY (collect_values[0].v_pointer != NULL))
    value->data[0].v_pointer = exo_object_ref (collect_values[0].v_pointer);

  return NULL;
}



static gchar*
exo_object_value_lcopy (const GValue *value,
                        guint         n_collect_values,
                        GTypeCValue  *collect_values,
                        guint         collect_flags)
{
  ExoObject **object_p = collect_values[0].v_pointer;

  g_return_val_if_fail (object_p != NULL, NULL);

  if (value->data[0].v_pointer == NULL)
    *object_p = NULL;
  else if (collect_flags & G_VALUE_NOCOPY_CONTENTS)
    *object_p = value->data[0].v_pointer;
  else
    *object_p = exo_object_ref (value->data[0].v_pointer);

  return NULL;
}



/**
 * exo_object_new:
 * @type : the type id of the #ExoObject subtype to instantiate.
 *
 * Creates a new instance of a #ExoObject subtype (as given by
 * the @type id).
 *
 * Return value: the new instance of @type.
 **/
gpointer
exo_object_new (GType type)
{
  g_return_val_if_fail (g_type_is_a (type, EXO_TYPE_OBJECT), NULL);
  return g_type_create_instance (type);
}



/**
 * exo_object_ref:
 * @object : an #ExoObject.
 *
 * Increments the reference count on @object by 1
 * in an atomic fashion and returns a pointer to
 * @object.
 *
 * Return value: a reference to @object.
 **/
gpointer
exo_object_ref (gpointer object)
{
  g_return_val_if_fail (EXO_IS_OBJECT (object), NULL);
  g_return_val_if_fail (EXO_OBJECT (object)->ref_count > 0, NULL);

#if defined(EXO_CPU_I386)
  __asm__ __volatile__ ("lock; incl %0" : "=m" (EXO_OBJECT (object)->ref_count) : "m" (EXO_OBJECT (object)->ref_count));
#else
  g_atomic_int_inc (&EXO_OBJECT (object)->ref_count);
#endif

  return object;
}



/**
 * exo_object_unref:
 * @object : an #ExoObject.
 *
 * Decrements the reference count on @object by
 * 1. If the reference count drops to zero, the
 * resources allocated to @object will be freed.
 **/
void
exo_object_unref (gpointer object)
{
  g_return_if_fail (EXO_IS_OBJECT (object));
  g_return_if_fail (EXO_OBJECT (object)->ref_count > 0);

  if (g_atomic_int_dec_and_test (&EXO_OBJECT (object)->ref_count))
    {
      /* finalize the object */
      (*EXO_OBJECT_GET_CLASS (object)->finalize) (EXO_OBJECT (object));

      /* free the instance resources */
      g_type_free_instance ((GTypeInstance *) object);
    }
}




GType
exo_param_spec_object_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (GParamSpecClass),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        sizeof (ExoParamSpecObject),
        0,
        NULL,
        NULL,
      };

      type = g_type_register_static (G_TYPE_PARAM, "ExoParamSpecObject", &info, 0);
    }

  return type;
}



/**
 * exo_param_spec_object:
 * @name        : canonical name of the property specified.
 * @nick        : nick name of the property specified.
 * @blurb       : description of the property specified.
 * @object_type : #EXO_TYPE_OBJECT derived type of this property.
 * @flags       : flags for the property specified.
 *
 * Creates a new #ExoParamSpecObject instance specifying a #EXO_TYPE_OBJECT
 * derived property.
 *
 * See #g_param_spec_internal() for details on the property names.
 *
 * Return value: the newly created parameter specification.
 **/
GParamSpec*
exo_param_spec_object (const gchar *name,
                       const gchar *nick,
                       const gchar *blurb,
                       GType        object_type,
                       GParamFlags  flags)
{
  GParamSpec *pspec;

  g_return_val_if_fail (g_type_is_a (object_type, EXO_TYPE_OBJECT), NULL);

  pspec = g_param_spec_internal (EXO_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
  pspec->value_type = object_type;

  return pspec;
}



/**
 * exo_value_set_object:
 * @value  : a valid #GValue of type #EXO_TYPE_OBJECT or a derived type.
 * @object : the #ExoObject to set or %NULL.
 *
 * Sets the contents of @value to @object.
 **/
void
exo_value_set_object (GValue  *value,
                      gpointer object)
{
  g_return_if_fail (EXO_VALUE_HOLDS_OBJECT (value));
  g_return_if_fail (object == NULL || EXO_IS_OBJECT (object));
  g_return_if_fail (object == NULL || g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value)));

  if (value->data[0].v_pointer != NULL)
    exo_object_unref (value->data[0].v_pointer);

  value->data[0].v_pointer = object;

  if (G_LIKELY (object != NULL))
    exo_object_ref (object);
}



/**
 * exo_value_take_object:
 * @value  : a valid #GValue of type #EXO_TYPE_OBJECT or a derived type.
 * @object : the #ExoObject to take over or %NULL.
 *
 * Sets the contents of @value to @object and takes over the ownership of
 * the callers reference to @object; the caller doesn't have to unref it
 * any more.
 **/
void
exo_value_take_object (GValue  *value,
                       gpointer object)
{
  g_return_if_fail (EXO_VALUE_HOLDS_OBJECT (value));
  g_return_if_fail (object == NULL || EXO_IS_OBJECT (object));
  g_return_if_fail (object == NULL || g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value)));

  if (value->data[0].v_pointer != NULL)
    exo_object_unref (value->data[0].v_pointer);

  value->data[0].v_pointer = object;
}



/**
 * exo_value_get_object:
 * @value : a valid #GValue of type #EXO_TYPE_OBJECT or a derived type.
 *
 * Queries the #ExoObject stored within @value and returns it. The
 * stored value may be %NULL.
 *
 * Return value: the #ExoObject stored in @value.
 **/
gpointer
exo_value_get_object (const GValue *value)
{
  g_return_val_if_fail (EXO_VALUE_HOLDS_OBJECT (value), NULL);
  return value->data[0].v_pointer;
}



/**
 * exo_value_dup_object:
 * @value : a valid #GValue of type #EXO_TYPE_OBJECT or a derived type.
 *
 * Similar to #exo_value_get_object(), but also takes a reference for
 * the caller if @value contains a valid #ExoObject.
 *
 * Return value: the #ExoObject stored in @value with an additional
 *               reference taken for the caller.
 **/
gpointer
exo_value_dup_object (const GValue *value)
{
  g_return_val_if_fail (EXO_VALUE_HOLDS_OBJECT (value), NULL);
  return (value->data[0].v_pointer != NULL) ? exo_object_ref (value->data[0].v_pointer) : NULL;
}



#define __EXO_OBJECT_C__
#include <exo/exo-aliasdef.c>
