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

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <exo/exo-private.h>
#include <exo-thumbnailers/exo-generic-thumbnailer.h>
#include <exo-thumbnailers/exo-thumbnailer-service.h>



#define EXO_THUMBNAILER_SERVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_THUMBNAILER_SERVICE, ExoThumbnailerServicePrivate))



/* Property identifiers */
enum
{
  PROP_0,
};



static void              exo_thumbnailer_service_class_init             (ExoThumbnailerServiceClass *klass);
static void              exo_thumbnailer_service_init                   (ExoThumbnailerService      *service);
static void              exo_thumbnailer_service_constructed            (GObject                    *object);
static void              exo_thumbnailer_service_finalize               (GObject                    *object);
static void              exo_thumbnailer_service_get_property           (GObject                    *object,
                                                                         guint                       prop_id,
                                                                         GValue                     *value,
                                                                         GParamSpec                 *pspec);
static void              exo_thumbnailer_service_set_property           (GObject                    *object,
                                                                         guint                       prop_id,
                                                                         const GValue               *value,
                                                                         GParamSpec                 *pspec);
static gboolean          exo_thumbnailer_service_start                  (ExoThumbnailerService      *service,
                                                                         GError                    **error);
static DBusHandlerResult exo_thumbnailer_service_handle_dbus_disconnect (DBusConnection             *connection,
                                                                         DBusMessage                *message,
                                                                         void                       *user_data);



struct _ExoThumbnailerServiceClass
{
  GObjectClass __parent__;
};

struct _ExoThumbnailerService
{
  GObject __parent__;

  ExoThumbnailerServicePrivate *priv;
};

struct _ExoThumbnailerServicePrivate
{
  DBusGConnection       *connection;
  ExoGenericThumbnailer *thumbnailer;
};



static GObjectClass *exo_thumbnailer_service_parent_class = NULL;



GType
exo_thumbnailer_service_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = g_type_register_static_simple (G_TYPE_OBJECT, 
                                            "ExoThumbnailerService",
                                            sizeof (ExoThumbnailerServiceClass),
                                            (GClassInitFunc) exo_thumbnailer_service_class_init,
                                            sizeof (ExoThumbnailerService),
                                            (GInstanceInitFunc) exo_thumbnailer_service_init,
                                            0);
    }

  return type;
}



static void
exo_thumbnailer_service_class_init (ExoThumbnailerServiceClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoThumbnailerServicePrivate));

  /* Determine the parent type class */
  exo_thumbnailer_service_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->constructed = exo_thumbnailer_service_constructed; 
  gobject_class->finalize = exo_thumbnailer_service_finalize; 
  gobject_class->get_property = exo_thumbnailer_service_get_property;
  gobject_class->set_property = exo_thumbnailer_service_set_property;
}



static void
exo_thumbnailer_service_init (ExoThumbnailerService *service)
{
  service->priv = EXO_THUMBNAILER_SERVICE_GET_PRIVATE (service);
  service->priv->connection = NULL;
  service->priv->thumbnailer = exo_generic_thumbnailer_get_default ();
}



static void
exo_thumbnailer_service_constructed (GObject *object)
{
}



static void
exo_thumbnailer_service_finalize (GObject *object)
{
  ExoThumbnailerService *service = EXO_THUMBNAILER_SERVICE (object);

  g_object_unref (service->priv->thumbnailer);

  if (service->priv->connection != NULL)
    dbus_g_connection_unref (service->priv->connection);

  (*G_OBJECT_CLASS (exo_thumbnailer_service_parent_class)->finalize) (object);
}



static void
exo_thumbnailer_service_get_property (GObject    *object,
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
exo_thumbnailer_service_set_property (GObject      *object,
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



static gboolean
exo_thumbnailer_service_start (ExoThumbnailerService *service,
                               GError               **error)
{
  DBusError dbus_error;
  gint      result;

  _exo_return_val_if_fail (EXO_IS_THUMBNAILER_SERVICE (service), FALSE);
  _exo_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  service->priv->connection = dbus_g_bus_get (DBUS_BUS_SESSION, error);
  if (G_UNLIKELY (service->priv->connection == NULL))
    return FALSE;

  dbus_g_connection_register_g_object (service->priv->connection, "/org/freedesktop/Thumbnailer",
                                       G_OBJECT (service->priv->thumbnailer));

  dbus_connection_add_filter (dbus_g_connection_get_connection (service->priv->connection),
                              exo_thumbnailer_service_handle_dbus_disconnect,
                              service, NULL);

  dbus_error_init (&dbus_error);

  result = dbus_bus_request_name (dbus_g_connection_get_connection (service->priv->connection),
                                  "org.freedesktop.thumbnailer.Generic", 
                                  DBUS_NAME_FLAG_DO_NOT_QUEUE, &dbus_error);

  if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
      if (dbus_error_is_set (&dbus_error))
        {
          if (error != NULL)
            dbus_set_g_error (error, &dbus_error);

          dbus_error_free (&dbus_error);
        }
      else if (error != NULL)
        {
          g_set_error (error, DBUS_GERROR, DBUS_GERROR_FAILED,
                       _("Another org.freedesktop.thumbnailer.Generic is already running"));
        }

      return FALSE;
    }

  return TRUE;
}



static DBusHandlerResult 
exo_thumbnailer_service_handle_dbus_disconnect (DBusConnection *connection,
                                                DBusMessage    *message,
                                                void           *user_data)
{
  g_debug ("disconnect");
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}



ExoThumbnailerService *
exo_thumbnailer_service_new_unique (GError **error)
{
  ExoThumbnailerService *service = NULL;

  _exo_return_val_if_fail (error == NULL || *error == NULL, NULL);

  service = g_object_new (EXO_TYPE_THUMBNAILER_SERVICE, NULL);

  if (!exo_thumbnailer_service_start (service, error))
    {
      g_object_unref (service);
      return NULL;
    }

  return service;
}
