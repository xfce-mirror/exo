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
#include <pygtk/pygtk.h>


extern void exo_add_constants (PyObject *module, const gchar *strip_prefix);
extern void exo_register_classes (PyObject *d);

extern PyMethodDef exo_functions[];


/* Unfortunately pygtk doesn't export the following functions,
 * that are required for the IconView, so we have to duplicate
 * them here.
 */
PyObject*
pygtk_tree_path_to_pyobject (GtkTreePath *path)
{
  gint len, i, *indices;
  PyObject *ret;

  len = gtk_tree_path_get_depth (path);
  indices = gtk_tree_path_get_indices (path);

  ret = PyTuple_New (len);
  for (i = 0; i < len; i++)
    PyTuple_SetItem (ret, i, PyInt_FromLong (indices[i]));
  return ret;
}

GtkTreePath*
pygtk_tree_path_from_pyobject (PyObject *object)
{
  if (PyString_Check(object))
    {
      GtkTreePath *path;
      
      path = gtk_tree_path_new_from_string (PyString_AsString (object));
      return path;
    }
  else if (PyInt_Check(object))
    {
      GtkTreePath *path;
      
      path = gtk_tree_path_new();
      gtk_tree_path_append_index(path, PyInt_AsLong(object));
      return path;
    }
  else if (PyTuple_Check(object))
    {
      GtkTreePath *path;
      guint len, i;
      
      len = PyTuple_Size(object);
      if (len < 1)
        return NULL;

      path = gtk_tree_path_new();
      for (i = 0; i < len; i++)
        {
          PyObject *item = PyTuple_GetItem(object, i);
          gint index = PyInt_AsLong(item);
          
          if (PyErr_Occurred())
            {
              gtk_tree_path_free(path);
              PyErr_Clear();
              return NULL;
            }
          gtk_tree_path_append_index(path, index);
        }

      return path;
    }
  
  return NULL;
}


DL_EXPORT(void)
init_exo (void)
{
  PyObject *d;
  PyObject *m;

  init_pygobject ();

  m = Py_InitModule ("_exo", exo_functions);
  d = PyModule_GetDict (m);

  exo_register_classes (d);
  exo_add_constants (m, "EXO_");

  if (PyErr_Occurred ())
    {
      Py_FatalError ("cannot initialize module _exo");
    }
}
