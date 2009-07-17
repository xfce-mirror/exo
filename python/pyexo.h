/* $Id$ */
/*-
 * Copyright (c) 2005-2006 os-cillation e.K.
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

#ifndef __PYEXO_H__
#define __PYEXO_H__

#include <pygobject.h>
#include <pygtk/pygtk.h>

#include <exo/exo.h>

G_BEGIN_DECLS

/* ---------- ExoBinding ---------- */
typedef struct
{
  PyObject_HEAD;
  ExoBinding *binding;
  PyObject   *transform_func;
} PyExoBinding;

extern PyTypeObject PyExoBinding_Type;

/* ---------- ExoMutualBinding ---------- */
typedef struct
{
  PyObject_HEAD;
  ExoMutualBinding *binding;
  PyObject         *transform_func;
  PyObject         *rtransform_func;
} PyExoMutualBinding;

extern PyTypeObject PyExoMutualBinding_Type;

/* ---------- PyGTK helpers ---------- */
#if !defined(pygtk_tree_path_from_pyobject)
G_GNUC_INTERNAL GtkTreePath *pygtk_tree_path_from_pyobject (PyObject *object);
#endif

#if !defined(pygtk_tree_path_to_pyobject)
G_GNUC_INTERNAL PyObject *pygtk_tree_path_to_pyobject (GtkTreePath *path);
#endif

G_END_DECLS

#endif /* !__PYEXO_H__ */
