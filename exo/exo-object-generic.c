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




