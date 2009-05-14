/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2009 Jannis Pohlmann <jannis@xfce.org>
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free 
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib-object.h>

#include <exo-thumbnailers/exo-generic-thumbnailer.h>
#include <exo-thumbnailers/exo-generic-thumbnailer-dbus-bindings.h>



#define EXO_GENERIC_THUMBNAILER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_GENERIC_THUMBNAILER, ExoGenericThumbnailerPrivate))



/* Property identifiers */
enum
{
  PROP_0,
};



static void     exo_generic_thumbnailer_class_init   (ExoGenericThumbnailerClass *klass);
static void     exo_generic_thumbnailer_init         (ExoGenericThumbnailer      *thumbnailer);
static void     exo_generic_thumbnailer_constructed  (GObject                    *object);
static void     exo_generic_thumbnailer_finalize     (GObject                    *object);
static void     exo_generic_thumbnailer_get_property (GObject                    *object,
                                                      guint                       prop_id,
                                                      GValue                     *value,
                                                      GParamSpec                 *pspec);
static void     exo_generic_thumbnailer_set_property (GObject                    *object,
                                                      guint                       prop_id,
                                                      const GValue               *value,
                                                      GParamSpec                 *pspec);



struct _ExoGenericThumbnailerClass
{
  GObjectClass __parent__;
};

struct _ExoGenericThumbnailer
{
  GObject __parent__;

  ExoGenericThumbnailerPrivate *priv;
};

struct _ExoGenericThumbnailerPrivate
{
  gint placeholder;
};



static GObjectClass *exo_generic_thumbnailer_parent_class = NULL;



GType
exo_generic_thumbnailer_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = g_type_register_static_simple (G_TYPE_OBJECT, 
                                            "ExoGenericThumbnailer",
                                            sizeof (ExoGenericThumbnailerClass),
                                            (GClassInitFunc) exo_generic_thumbnailer_class_init,
                                            sizeof (ExoGenericThumbnailer),
                                            (GInstanceInitFunc) exo_generic_thumbnailer_init,
                                            0);
    }

  return type;
}



static void
exo_generic_thumbnailer_class_init (ExoGenericThumbnailerClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoGenericThumbnailerPrivate));

  /* Determine the parent type class */
  exo_generic_thumbnailer_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->constructed = exo_generic_thumbnailer_constructed; 
  gobject_class->finalize = exo_generic_thumbnailer_finalize; 
  gobject_class->get_property = exo_generic_thumbnailer_get_property;
  gobject_class->set_property = exo_generic_thumbnailer_set_property;

  dbus_g_object_type_install_info (G_TYPE_FROM_CLASS (klass),
                                   &dbus_glib_exo_generic_thumbnailer_object_info);
}



static void
exo_generic_thumbnailer_init (ExoGenericThumbnailer *thumbnailer)
{
  thumbnailer->priv = EXO_GENERIC_THUMBNAILER_GET_PRIVATE (thumbnailer);
}



static void
exo_generic_thumbnailer_constructed (GObject *object)
{
}



static void
exo_generic_thumbnailer_finalize (GObject *object)
{
  (*G_OBJECT_CLASS (exo_generic_thumbnailer_parent_class)->finalize) (object);
}



static void
exo_generic_thumbnailer_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_generic_thumbnailer_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



ExoGenericThumbnailer *
exo_generic_thumbnailer_get_default (void)
{
  static ExoGenericThumbnailer *thumbnailer = NULL;

  if (G_UNLIKELY (thumbnailer == NULL))
    {
      thumbnailer = g_object_new (EXO_TYPE_GENERIC_THUMBNAILER, NULL);
      g_object_add_weak_pointer (G_OBJECT (thumbnailer), (gpointer) &thumbnailer);
    }
  else
    {
      g_object_ref (thumbnailer);
    }

  return thumbnailer;
}



gboolean
exo_generic_thumbnailer_queue (ExoGenericThumbnailer *thumbnailer,
                               const gchar          **uris,
                               const gchar          **mime_hints,
                               guint32                unqueue_handle,
                               guint32               *handle,
                               GError               **error)
{
  gint n;

  g_debug ("exo_generic_thumbnailer_queue:");

  return TRUE;

  g_debug ("  uris = ");
  for (n = 0; uris[n] != NULL; ++n)
    g_debug ("    %s", uris[n]);

  g_debug ("  mime_hints = ");
  for (n = 0; mime_hints[n] != NULL; ++n)
    g_debug ("    %s", mime_hints[n]);

  return TRUE;
}



gboolean 
exo_generic_thumbnailer_test (ExoGenericThumbnailer *thumbnailer,
                              GError               **error)
{
  g_debug ("Test");
  return TRUE;
}
