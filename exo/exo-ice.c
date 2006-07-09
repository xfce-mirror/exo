/* $Id$ */
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
 *
 * Based on the ice-layer module of xfce4-session and the gnome-ice
 * module of libgnomeui.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_LIBSM
#include <X11/ICE/ICElib.h>
#endif

#include <glib-object.h>

#include <exo/exo-ice.h>



#ifdef HAVE_LIBSM

static IceIOErrorHandler exo_ice_installed_handler = NULL;



static void
ice_error_handler (IceConn ice_conn)
{
  /*
   * The I/O error handlers does whatever is necessary to respond
   * to the I/O error and then returns, but it does not call
   * IceCloseConnection. The ICE connection is given a "bad IO"
   * status, and all future reads and writes to the connection
   * are ignored. The next time IceProcessMessages is called it
   * will return a status of IceProcessMessagesIOError. At that
   * time, the application should call IceCloseConnection.
   */
  if (exo_ice_installed_handler != NULL)
    exo_ice_installed_handler (ice_conn);
}



static gboolean
ice_process_messages (GIOChannel  *channel,
                      GIOCondition condition,
                      gpointer     user_data)
{
  IceProcessMessagesStatus status;
  IcePointer               ice_context;
  IceConn                  ice_conn = user_data;
  guint                    disconnect_id;

  status = IceProcessMessages (ice_conn, NULL, NULL);

  if (status == IceProcessMessagesIOError)
    {
      ice_context = IceGetConnectionContext (ice_conn);

      if (ice_context != NULL && G_IS_OBJECT (ice_context))
        {
          disconnect_id = g_signal_lookup ("disconnect", G_OBJECT_TYPE (ice_context));
          if (disconnect_id > 0)
            g_signal_emit (G_OBJECT (ice_context), disconnect_id, 0);
        }
      else
        {
          IceSetShutdownNegotiation (ice_conn, False);
          IceCloseConnection (ice_conn);
        }
    }

  return TRUE;
}



static void
ice_connection_watch (IceConn     ice_conn,
                      IcePointer  client_data,
                      Bool        opening,
                      IcePointer *watch_data)
{
  GIOChannel *channel;
  guint       watch_id;
  gint        fd;

  if (opening)
    {
      fd = IceConnectionNumber (ice_conn);

      /* Make sure we don't pass on these file descriptors to
       * an exec'd child process.
       */
      fcntl (fd, F_SETFD, fcntl (fd, F_GETFD, 0) | FD_CLOEXEC);

      channel = g_io_channel_unix_new (fd);
      watch_id = g_io_add_watch (channel, G_IO_ERR | G_IO_HUP | G_IO_IN,
                                 ice_process_messages, ice_conn);
      g_io_channel_unref (channel);

      *watch_data = GUINT_TO_POINTER (watch_id);
    }
  else
    {
      watch_id = GPOINTER_TO_UINT (*watch_data);
      g_source_remove (watch_id);
    }
}

#endif /* HAVE_LIBSM */



/**
 * exo_ice_init:
 *
 * This function should be called before you use any ICE functions.
 * It will arrange for ICE connections to be read and dispatched via
 * the Glib event loop mechanism. This function can be called any number
 * of times without any harm.
 **/
void
exo_ice_init (void)
{
#ifdef HAVE_LIBSM
  IceIOErrorHandler default_handler;
  static gboolean   inited = FALSE;

  if (!inited)
    {
      exo_ice_installed_handler = IceSetIOErrorHandler (NULL);
      default_handler = IceSetIOErrorHandler (ice_error_handler);

      if (exo_ice_installed_handler == default_handler)
        exo_ice_installed_handler = NULL;

      IceAddConnectionWatch (ice_connection_watch, NULL);

      inited = TRUE;
    }
#endif
}

