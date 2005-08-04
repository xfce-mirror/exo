/* $Id$ */
/*-
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <exo/exo.h>



static gboolean init_called = FALSE;
static gboolean finalize_called = FALSE;



typedef struct _FooBarClass FooBarClass;
typedef struct _FooBar      FooBar;



#define FOO_TYPE_BAR    (foo_bar_get_type ())
#define FOO_BAR(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), FOO_TYPE_BAR, FooBar))
#define FOO_IS_BAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FOO_TYPE_BAR))



struct _FooBarClass
{
  ExoObjectClass __parent__;
};

struct _FooBar
{
  ExoObject __parent__;
  gchar  *foo;
};



static void foo_bar_class_init (FooBarClass *klass);
static void foo_bar_init       (FooBar      *bar);
static void foo_bar_finalize   (ExoObject     *object);



G_DEFINE_TYPE (FooBar, foo_bar, EXO_TYPE_OBJECT);



static void
foo_bar_class_init (FooBarClass *klass)
{
  ExoObjectClass *exoobject_class;

  foo_bar_parent_class = g_type_class_peek_parent (klass);

  exoobject_class = EXO_OBJECT_CLASS (klass);
  exoobject_class->finalize = foo_bar_finalize;
}



static void
foo_bar_init (FooBar *bar)
{
  bar->foo = g_strdup ("Foo Bar");
  init_called = TRUE;
}



static void
foo_bar_finalize (ExoObject *object)
{
  FooBar *bar = FOO_BAR (object);
  g_free (bar->foo);
  EXO_OBJECT_CLASS (foo_bar_parent_class)->finalize (object);
  finalize_called = TRUE;
}



int
main (int argc, char **argv)
{
  ExoObject *object;
  FooBar  *bar;

  g_type_init ();

  assert (!init_called);
  bar = exo_object_new (FOO_TYPE_BAR);
  assert (init_called);
  assert (FOO_IS_BAR (bar));
  assert (EXO_OBJECT (bar)->ref_count == 1);

  assert (exo_str_is_equal (bar->foo, "Foo Bar"));

  object = exo_object_ref (EXO_OBJECT (bar));
  assert (EXO_OBJECT (bar)->ref_count == 2);
  assert (FOO_IS_BAR (object));
  assert (FOO_IS_BAR (bar));

  object = exo_object_ref (EXO_OBJECT (bar));
  assert (EXO_OBJECT (bar)->ref_count == 3);
  assert (FOO_IS_BAR (object));
  assert (FOO_IS_BAR (bar));

  exo_object_unref (EXO_OBJECT (bar));
  assert (EXO_OBJECT (bar)->ref_count == 2);
  assert (FOO_IS_BAR (bar));

  exo_object_unref (EXO_OBJECT (bar));
  assert (EXO_OBJECT (bar)->ref_count == 1);
  assert (FOO_IS_BAR (bar));

  assert (!finalize_called);
  exo_object_unref (EXO_OBJECT (bar));
  assert (finalize_called);
  assert (((ExoObject *) bar)->ref_count == 0);
  assert (((GTypeInstance *) bar)->g_class == NULL);

  return EXIT_SUCCESS;
}


