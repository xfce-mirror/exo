/* $Id$ */
/*-
 * Copyright (c) 2005 os-cillation e.K.
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

#include <pygobject.h>


extern void exo_register_classes (PyObject *d);
extern PyMethodDef exo_functions[];


DL_EXPORT(void)
init_exo (void)
{
  PyObject *d;
  PyObject *m;

  init_pygobject ();

  m = Py_InitModule ("_exo", exo_functions);
  d = PyModule_GetDict (m);

  exo_register_classes (d);

  if (PyErr_Occurred ())
    {
      Py_FatalError ("cannot initialize module _exo");
    }
}
