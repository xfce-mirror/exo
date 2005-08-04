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

#include <exo/exo-object.h>



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

  g_atomic_int_inc (&EXO_OBJECT (object)->ref_count);

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




