/* $Id: exo-dbus.c,v 1.1.1.1 2004/09/14 22:32:58 bmeurer Exp $ */
/*-
 * Copyright (c) 2004 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <exo/exo-dbus.h>



static DBusGConnection *default_connection = NULL;



/**
 * exo_dbus_bus_connection:
 *
 * Return value :
 **/
DBusConnection*
exo_dbus_bus_connection (void)
{
  GError *error;

  if (G_UNLIKELY (default_connection == NULL))
    {
      error = NULL;
      default_connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
      if (G_UNLIKELY (default_connection == NULL))
        {
          g_warning ("Failed to open connection to D-BUS message bus: %s",
                     error->message);
          g_error_free (error);
          return NULL;
        }
    }

  return dbus_g_connection_get_connection (default_connection);
}
