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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pyexo.h"



static void      pyexo_binding_dealloc            (PyExoBinding       *self);
static gint      pyexo_binding_compare            (PyExoBinding       *self,
                                                   PyExoBinding       *other);
static glong     pyexo_binding_hash               (PyExoBinding       *self);
static PyObject *pyexo_binding_repr               (PyExoBinding       *self);
static gint      pyexo_binding_init               (PyExoBinding       *self,
                                                   PyObject           *args,
                                                   PyObject           *kwargs);
static PyObject *pyexo_binding_is_bound           (PyExoBinding       *self);
static PyObject *pyexo_binding_unbind             (PyExoBinding       *self);
static void      pyexo_binding_destroy            (PyExoBinding       *self);
static gboolean  pyexo_binding_transform          (const GValue       *src_value,
                                                   GValue             *dst_value,
                                                   PyExoBinding       *self);
static void      pyexo_mutual_binding_dealloc     (PyExoMutualBinding *self);
static gint      pyexo_mutual_binding_compare     (PyExoMutualBinding *self,
                                                   PyExoMutualBinding *other);
static glong     pyexo_mutual_binding_hash        (PyExoMutualBinding *self);
static PyObject *pyexo_mutual_binding_repr        (PyExoMutualBinding *self);
static gint      pyexo_mutual_binding_init        (PyExoMutualBinding *self,
                                                   PyObject           *args,
                                                   PyObject           *kwargs);
static PyObject *pyexo_mutual_binding_is_bound    (PyExoMutualBinding *self);
static PyObject *pyexo_mutual_binding_unbind      (PyExoMutualBinding *self);
static void      pyexo_mutual_binding_destroy     (PyExoMutualBinding *self);
static gboolean  pyexo_mutual_binding_transform   (const GValue       *src_value,
                                                   GValue             *dst_value,
                                                   PyExoMutualBinding *self);
static gboolean  pyexo_mutual_binding_rtransform  (const GValue       *src_value,
                                                   GValue             *dst_value,
                                                   PyExoMutualBinding *self);



static PyMethodDef pyexo_binding_methods[] =
{
  { "is_bound", (PyCFunction) pyexo_binding_is_bound, METH_NOARGS, },
  { "unbind",   (PyCFunction) pyexo_binding_unbind,   METH_NOARGS, },
  { NULL,       NULL,                                 0,           },
};

static PyMethodDef pyexo_mutual_binding_methods[] =
{
  { "is_bound", (PyCFunction) pyexo_mutual_binding_is_bound, METH_NOARGS, },
  { "unbind",   (PyCFunction) pyexo_mutual_binding_unbind,   METH_NOARGS, },
  { NULL,       NULL,                                        0,           },
};



PyTypeObject PyExoBinding_Type =
{
  PyObject_HEAD_INIT(NULL)
  0,
  "exo.Binding",
  sizeof (PyExoBinding),
  0,
  (destructor) pyexo_binding_dealloc,
  (printfunc) NULL,
  (getattrfunc) NULL,
  (setattrfunc) NULL,
  (cmpfunc) pyexo_binding_compare,
  (reprfunc) pyexo_binding_repr,
  0,
  0,
  0,
  (hashfunc) pyexo_binding_hash,
  (ternaryfunc) NULL,
  (reprfunc) NULL,
  (getattrofunc) NULL,
  (setattrofunc) NULL,
  0,
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  NULL,
  (traverseproc) NULL,
  (inquiry) NULL,
  (richcmpfunc) NULL,
  0,
  (getiterfunc) NULL,
  (iternextfunc) NULL,
  pyexo_binding_methods,
  0,
  0,
  NULL,
  NULL,
  (descrgetfunc) NULL,
  (descrsetfunc) NULL,
  0,
  (initproc) pyexo_binding_init,
};

PyTypeObject PyExoMutualBinding_Type =
{
  PyObject_HEAD_INIT(NULL)
  0,
  "exo.MutualBinding",
  sizeof (PyExoMutualBinding),
  0,
  (destructor) pyexo_mutual_binding_dealloc,
  (printfunc) NULL,
  (getattrfunc) NULL,
  (setattrfunc) NULL,
  (cmpfunc) pyexo_mutual_binding_compare,
  (reprfunc) pyexo_mutual_binding_repr,
  0,
  0,
  0,
  (hashfunc) pyexo_mutual_binding_hash,
  (ternaryfunc) NULL,
  (reprfunc) NULL,
  (getattrofunc) NULL,
  (setattrofunc) NULL,
  0,
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  NULL,
  (traverseproc) NULL,
  (inquiry) NULL,
  (richcmpfunc) NULL,
  0,
  (getiterfunc) NULL,
  (iternextfunc) NULL,
  pyexo_mutual_binding_methods,
  0,
  0,
  NULL,
  NULL,
  (descrgetfunc) NULL,
  (descrsetfunc) NULL,
  0,
  (initproc) pyexo_mutual_binding_init,
};



static void
pyexo_binding_dealloc (PyExoBinding *self)
{
  /* release transform func */
  Py_XDECREF (self->transform_func);

  PyObject_Del (self);
}



static gint
pyexo_binding_compare (PyExoBinding *self,
                       PyExoBinding *other)
{
  if (self->binding == other->binding)
    return 0;
  else if (self->binding > other->binding)
    return -1;
  else
    return 1;
}



static glong
pyexo_binding_hash (PyExoBinding *self)
{
  return (glong) self->binding;
}



static PyObject*
pyexo_binding_repr (PyExoBinding *self)
{
  gchar repr[128];
  g_snprintf (repr, sizeof (repr), "<Binding at 0x%lx>", (glong) self->binding);
  return PyString_FromString (repr);
}



static gint
pyexo_binding_init (PyExoBinding *self,
                    PyObject     *args,
                    PyObject     *kwargs)
{

  const gchar  *kwlist[] = { "src_object", "src_property", "dst_object", "dst_property", "transform_func", NULL };
  const gchar  *src_property;
  const gchar  *dst_property;
  PyObject     *transform_func = Py_None;
  PyObject     *gobject_module;
  PyObject     *gobject_type;
  PyObject     *src_object;
  PyObject     *dst_object;
  gchar        *message;

  /* parse the constructor arguments */
  if (!PyArg_ParseTupleAndKeywords (args, kwargs, "OsOs|O:Binding.__init__", (gchar **) kwlist,
                                    &src_object, &src_property, &dst_object, &dst_property,
                                    &transform_func))
    {
      return -1;
    }

  /* import module gobject */
  gobject_module = PyImport_ImportModule ("gobject");
  if (G_UNLIKELY (gobject_module == NULL))
    {
      PyErr_SetString (PyExc_ImportError, "could not import gobject");
      return -1;
    }

  /* lookup type gobject.GObject */
  gobject_type = PyDict_GetItemString (PyModule_GetDict (gobject_module), "GObject");
  if (G_UNLIKELY (gobject_type == NULL))
    {
      PyErr_SetString (PyExc_ImportError, "cannot import name GObject from gobject");
      return -1;
    }

  /* validate the constructor arguments */
  if (!pygobject_check (src_object, (PyTypeObject *) gobject_type) || src_object == Py_None)
    {
      PyErr_SetString (PyExc_TypeError, "src_object must be a gobject.GObject");
      return -1;
    }
  else if (!pygobject_check (dst_object, (PyTypeObject *) gobject_type) || dst_object == Py_None)
    {
      PyErr_SetString (PyExc_TypeError, "dst_object must be a gobject.GObject");
      return -1;
    }
  else if (transform_func != Py_None && !PyCallable_Check (transform_func))
    {
      PyErr_SetString (PyExc_TypeError, "transform_func must be a callback object or None");
      return -1;
    }

  /* setup transform func */
  self->transform_func = transform_func;
  Py_INCREF (transform_func);

  /* allocate the binding */
  self->binding = exo_binding_new_full (pygobject_get (src_object), src_property, pygobject_get (dst_object), dst_property,
                                        (transform_func != Py_None) ? (ExoBindingTransform) pyexo_binding_transform : NULL,
                                        (GDestroyNotify) pyexo_binding_destroy, self);
  if (G_UNLIKELY (self->binding == NULL))
    {
      message = g_strdup_printf ("cannot bind property %s to property %s", src_property, dst_property);
      PyErr_SetString (PyExc_TypeError, message);
      g_free (message);
      return -1;
    }

  /* take a reference, which will be released
   * when the real binding is destroyed.
   */
  Py_INCREF (self);

  return 0;
}



static PyObject*
pyexo_binding_is_bound (PyExoBinding *self)
{
  return PyBool_FromLong (self->binding != NULL);
}



static PyObject*
pyexo_binding_unbind (PyExoBinding *self)
{
  /* check if the binding is still active */
  if (G_LIKELY (self->binding != NULL))
    exo_binding_unbind (self->binding);

  Py_INCREF (Py_None);
  return Py_None;
}



static void
pyexo_binding_destroy (PyExoBinding *self)
{
  /* make sure that the binding was still active */
  if (G_LIKELY (self->binding != NULL))
    {
      /* reset to indicate that its invalid now */
      self->binding = NULL;

      /* release the extra reference on the Python object */
      Py_DECREF (self);
    }
}



static gboolean
pyexo_binding_transform (const GValue *src_value,
                         GValue       *dst_value,
                         PyExoBinding *self)
{
  PyGILState_STATE state;
  gboolean         result = FALSE;
  PyObject        *src_object;
  PyObject        *dst_object;

  state = pyg_gil_state_ensure ();

  src_object = pyg_value_as_pyobject (src_value, TRUE);
  if (G_LIKELY (src_object != NULL))
    {
      dst_object = PyEval_CallFunction (self->transform_func, "(O)", src_object);
      if (G_UNLIKELY (dst_object == NULL))
        {
          PyErr_Print ();
        }
      else
        {
          /* try to transform the result */
          result = (pyg_value_from_pyobject (dst_value, dst_object) == 0);

          /* release the result */
          Py_DECREF (dst_object);
        }

      Py_DECREF (src_object);
    }
  else
    {
      PyErr_Print ();
    }

  pyg_gil_state_release (state);

  return result;
}



static void
pyexo_mutual_binding_dealloc (PyExoMutualBinding *self)
{
  /* release reverse transform func */
  Py_XDECREF (self->rtransform_func);

  /* release transform func */
  Py_XDECREF (self->transform_func);

  PyObject_Del (self);
}



static gint
pyexo_mutual_binding_compare (PyExoMutualBinding *self,
                              PyExoMutualBinding *other)
{
  if (self->binding == other->binding)
    return 0;
  else if (self->binding > other->binding)
    return -1;
  else
    return 1;
}



static glong
pyexo_mutual_binding_hash (PyExoMutualBinding *self)
{
  return (glong) self->binding;
}



static PyObject*
pyexo_mutual_binding_repr (PyExoMutualBinding *self)
{
  gchar repr[128];
  g_snprintf (repr, sizeof (repr), "<MutualBinding at 0x%lx>", (glong) self->binding);
  return PyString_FromString (repr);
}



static gint
pyexo_mutual_binding_init (PyExoMutualBinding *self,
                           PyObject           *args,
                           PyObject           *kwargs)
{
  const gchar  *kwlist[] = { "src_object", "src_property", "dst_object", "dst_property", "transform_func", "reverse_transform_func", NULL };
  const gchar  *src_property;
  const gchar  *dst_property;
  PyObject     *rtransform_func = Py_None;
  PyObject     *transform_func = Py_None;
  PyObject     *gobject_module;
  PyObject     *gobject_type;
  PyObject     *src_object;
  PyObject     *dst_object;
  gchar        *message;

  /* parse the constructor arguments */
  if (!PyArg_ParseTupleAndKeywords (args, kwargs, "OsOs|OO:MutualBinding.__init__", (gchar **) kwlist,
                                    &src_object, &src_property, &dst_object, &dst_property,
                                    &transform_func, &rtransform_func))
    {
      return -1;
    }

  /* import module gobject */
  gobject_module = PyImport_ImportModule ("gobject");
  if (G_UNLIKELY (gobject_module == NULL))
    {
      PyErr_SetString (PyExc_ImportError, "could not import gobject");
      return -1;
    }

  /* lookup type gobject.GObject */
  gobject_type = PyDict_GetItemString (PyModule_GetDict (gobject_module), "GObject");
  if (G_UNLIKELY (gobject_type == NULL))
    {
      PyErr_SetString (PyExc_ImportError, "cannot import name GObject from gobject");
      return -1;
    }

  /* validate the constructor arguments */
  if (!pygobject_check (src_object, (PyTypeObject *) gobject_type) || src_object == Py_None)
    {
      PyErr_SetString (PyExc_TypeError, "src_object must be a gobject.GObject");
      return -1;
    }
  else if (!pygobject_check (dst_object, (PyTypeObject *) gobject_type) || dst_object == Py_None)
    {
      PyErr_SetString (PyExc_TypeError, "dst_object must be a gobject.GObject");
      return -1;
    }
  else if (transform_func != Py_None && !PyCallable_Check (transform_func))
    {
      PyErr_SetString (PyExc_TypeError, "transform_func must be a callable object or None");
      return -1;
    }
  else if (rtransform_func != Py_None && !PyCallable_Check (rtransform_func))
    {
      PyErr_SetString (PyExc_TypeError, "reverse_transform_func must be a callable object or None");
      return -1;
    }

  /* setup transform func */
  self->transform_func = transform_func;
  Py_INCREF (self->transform_func);

  /* setup reverse transform func */
  self->rtransform_func = rtransform_func;
  Py_INCREF (self->rtransform_func);

  /* allocate the binding */
  self->binding = exo_mutual_binding_new_full (pygobject_get (src_object), src_property, pygobject_get (dst_object), dst_property,
                                               (transform_func != Py_None) ? (ExoBindingTransform) pyexo_mutual_binding_transform : NULL,
                                               (rtransform_func != Py_None) ? (ExoBindingTransform) pyexo_mutual_binding_rtransform : NULL,
                                               (GDestroyNotify) pyexo_mutual_binding_destroy, self);
  if (G_UNLIKELY (self->binding == NULL))
    {
      message = g_strdup_printf ("cannot bind property %s to property %s", src_property, dst_property);
      PyErr_SetString (PyExc_TypeError, message);
      g_free (message);
      return -1;
    }

  /* take a reference, which will be released
   * when the real binding is destroyed.
   */
  Py_INCREF (self);

  return 0;
}



static PyObject*
pyexo_mutual_binding_is_bound (PyExoMutualBinding *self)
{
  return PyBool_FromLong (self->binding != NULL);
}



static PyObject*
pyexo_mutual_binding_unbind (PyExoMutualBinding *self)
{
  /* check if the binding is still active */
  if (G_LIKELY (self->binding != NULL))
    exo_mutual_binding_unbind (self->binding);

  Py_INCREF (Py_None);
  return Py_None;
}



static void
pyexo_mutual_binding_destroy (PyExoMutualBinding *self)
{
  /* make sure that the binding was still active */
  if (G_LIKELY (self->binding != NULL))
    {
      /* reset to indicate that its invalid now */
      self->binding = NULL;

      /* release the extra reference on the Python object */
      Py_DECREF (self);
    }
}



static gboolean
pyexo_mutual_binding_transform (const GValue       *src_value,
                                GValue             *dst_value,
                                PyExoMutualBinding *self)
{
  PyGILState_STATE state;
  gboolean         result = FALSE;
  PyObject        *src_object;
  PyObject        *dst_object;

  state = pyg_gil_state_ensure ();

  src_object = pyg_value_as_pyobject (src_value, TRUE);
  if (G_LIKELY (src_object != NULL))
    {
      dst_object = PyEval_CallFunction (self->transform_func, "(O)", src_object);
      if (G_UNLIKELY (dst_object == NULL))
        {
          PyErr_Print ();
        }
      else
        {
          /* try to transform the result */
          result = (pyg_value_from_pyobject (dst_value, dst_object) == 0);

          /* release the result */
          Py_DECREF (dst_object);
        }

      Py_DECREF (src_object);
    }
  else
    {
      PyErr_Print ();
    }

  pyg_gil_state_release (state);

  return result;
}



static gboolean
pyexo_mutual_binding_rtransform (const GValue       *src_value,
                                 GValue             *dst_value,
                                 PyExoMutualBinding *self)
{
  PyGILState_STATE state;
  gboolean         result = FALSE;
  PyObject        *src_object;
  PyObject        *dst_object;

  state = pyg_gil_state_ensure ();

  src_object = pyg_value_as_pyobject (src_value, TRUE);
  if (G_LIKELY (src_object != NULL))
    {
      dst_object = PyEval_CallFunction (self->rtransform_func, "(O)", src_object);
      if (G_UNLIKELY (dst_object == NULL))
        {
          PyErr_Print ();
        }
      else
        {
          /* try to transform the result */
          result = (pyg_value_from_pyobject (dst_value, dst_object) == 0);

          /* release the result */
          Py_DECREF (dst_object);
        }

      Py_DECREF (src_object);
    }
  else
    {
      PyErr_Print ();
    }

  pyg_gil_state_release (state);

  return result;
}

