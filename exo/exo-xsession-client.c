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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <X11/Xlib.h>

#include <gdk/gdkx.h>

#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-xsession-client.h>



#define EXO_XSESSION_CLIENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_XSESSION_CLIENT, ExoXsessionClientPrivate))



enum
{
  PROP_0,
  PROP_GROUP,
};

enum
{
  SAVE_YOURSELF,
  LAST_SIGNAL,
};



static void             exo_xsession_client_class_init    (ExoXsessionClientClass *klass);
static void             exo_xsession_client_init          (ExoXsessionClient      *client);
static void             exo_xsession_client_dispose       (GObject                *object);
static void             exo_xsession_client_get_property  (GObject                *object,
                                                           guint                   prop_id,
                                                           GValue                 *value,
                                                           GParamSpec             *pspec);
static void             exo_xsession_client_set_property  (GObject                *object,
                                                           guint                   prop_id,
                                                           const GValue           *value,
                                                           GParamSpec             *pspec);
static GdkFilterReturn  exo_xsession_client_filter        (GdkXEvent              *xevent,
                                                           GdkEvent               *event,
                                                           gpointer                data);



struct _ExoXsessionClientPrivate
{
  Atom        wm_protocols;
  Atom        wm_save_yourself;
  GdkWindow  *leader;
};



static GObjectClass *parent_class;
static guint         client_signals[LAST_SIGNAL];



G_DEFINE_TYPE (ExoXsessionClient, exo_xsession_client, G_TYPE_OBJECT);



static void
exo_xsession_client_class_init (ExoXsessionClientClass *klass)
{
  GObjectClass *gobject_class;

  /* initialize the libexo i18n support */
  _exo_i18n_init ();

  g_type_class_add_private (klass, sizeof (ExoXsessionClientPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = exo_xsession_client_dispose;
  gobject_class->get_property = exo_xsession_client_get_property;
  gobject_class->set_property = exo_xsession_client_set_property;

  /**
   * ExoXsessionClient:group:
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_GROUP,
                                   g_param_spec_object ("group",
                                                        _("Window group"),
                                                        _("Window group"),
                                                        GDK_TYPE_WINDOW,
                                                        G_PARAM_READWRITE));

  /**
   * ExoXsessionClient::save-yourself:
   * @client  : An #ExoXsessionClient.
   *
   * This signal is emitted when @client receives a %WM_SAVE_YOURSELF
   * message from the session manager or the window manager on the 
   * specified client leader window.
   **/
  client_signals[SAVE_YOURSELF] =
    g_signal_new ("save-yourself",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (ExoXsessionClientClass, save_yourself),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}



static void
exo_xsession_client_init (ExoXsessionClient *client)
{
  client->priv = EXO_XSESSION_CLIENT_GET_PRIVATE (client);
}



static void
exo_xsession_client_dispose (GObject *object)
{
  ExoXsessionClient *client = EXO_XSESSION_CLIENT (object);

  exo_xsession_client_set_group (client, NULL);

  G_OBJECT_CLASS (parent_class)->dispose (object);
}



static void
exo_xsession_client_get_property (GObject     *object,
                                  guint        prop_id,
                                  GValue      *value,
                                  GParamSpec  *pspec)
{
  ExoXsessionClient *client = EXO_XSESSION_CLIENT (object);

  switch (prop_id)
    {
    case PROP_GROUP:
      g_value_set_object (value, client->priv->leader);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_xsession_client_set_property (GObject       *object,
                                  guint          prop_id,
                                  const GValue  *value,
                                  GParamSpec    *pspec)
{
  ExoXsessionClient *client = EXO_XSESSION_CLIENT (object);

  switch (prop_id)
    {
    case PROP_GROUP:
      exo_xsession_client_set_group (client, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static GdkFilterReturn
exo_xsession_client_filter (GdkXEvent *xevent,
                            GdkEvent  *event,
                            gpointer   data)
{
  XClientMessageEvent *xev = (XClientMessageEvent *) xevent;
  ExoXsessionClient   *client = EXO_XSESSION_CLIENT (data);

  if (xev->type == ClientMessage
      && xev->message_type == client->priv->wm_protocols
      && xev->format == 32
      && xev->data.l[0] == client->priv->wm_save_yourself)
    {
      g_signal_emit (G_OBJECT (client), client_signals[SAVE_YOURSELF], 0);
      return GDK_FILTER_REMOVE;
    }

  return GDK_FILTER_CONTINUE;
}



/**
 * exo_xsession_client_new_with_group:
 * @leader  : The client leader window of the group.
 *
 * Creates a new #ExoXsessionClient and associates it
 * with the group, which is lead by @leader.
 *
 * Return value: A newly allocated #ExoXsessionClient.
 **/
ExoXsessionClient*
exo_xsession_client_new_with_group (GdkWindow *leader)
{
  g_return_val_if_fail (GDK_IS_WINDOW (leader), NULL);

  return g_object_new (EXO_TYPE_XSESSION_CLIENT,
                       "group", leader,
                       NULL);
}



/**
 * exo_xsession_client_set_group:
 * @client  : An #ExoXsessionClient.
 * @leader  : The client leader window of a group or %NULL.
 **/
void
exo_xsession_client_set_group (ExoXsessionClient *client,
                               GdkWindow         *leader)
{
  static char *atom_names[2] = { "WM_PROTOCOLS", "WM_SAVE_YOURSELF" };
  Atom         atoms[2];
  Atom        *protocols;
  Atom        *protocols_custom;
  int          nprotocols;
  int          n;
  int          m;

  g_return_if_fail (EXO_IS_XSESSION_CLIENT (client));
  g_return_if_fail (GDK_IS_WINDOW (leader) || leader == NULL);

  if (G_UNLIKELY (client->priv->leader == leader))
    return;

  if (client->priv->leader != NULL)
    {
      /* remove WM_SAVE_YOURSELF protocol property */
      if (XGetWMProtocols (GDK_DRAWABLE_XDISPLAY (client->priv->leader),
                           GDK_DRAWABLE_XID (client->priv->leader),
                           &protocols, &nprotocols))
        {
          for (n = 0, m = 0; n < nprotocols; ++n)
            if (protocols[n] != client->priv->wm_save_yourself)
              protocols[m++] = protocols[n];
          nprotocols = m;

          if (G_LIKELY (nprotocols > 0))
            {
              XSetWMProtocols (GDK_DRAWABLE_XDISPLAY (client->priv->leader),
                               GDK_DRAWABLE_XID (client->priv->leader),
                               protocols, nprotocols);
            }

          XFree ((void *) protocols);
        }

      gdk_window_remove_filter (client->priv->leader, exo_xsession_client_filter, client);
      g_object_unref (G_OBJECT (client->priv->leader));
    }

  client->priv->leader = leader;

  if (leader != NULL)
    {
      XInternAtoms (GDK_DRAWABLE_XDISPLAY (leader), atom_names, 2, False, atoms);
      client->priv->wm_protocols = atoms[0];
      client->priv->wm_save_yourself = atoms[1];

      /* setup WM_SAVE_YOURSELF protocol property */
      if (XGetWMProtocols (GDK_DRAWABLE_XDISPLAY (leader),
                           GDK_DRAWABLE_XID (leader),
                           &protocols, &nprotocols))
        {
          protocols_custom = g_new (Atom, nprotocols + 1);
          bcopy (protocols, protocols_custom, nprotocols * sizeof (*protocols));
          protocols_custom[nprotocols++] = client->priv->wm_save_yourself;

          XSetWMProtocols (GDK_DRAWABLE_XDISPLAY (leader),
                           GDK_DRAWABLE_XID (leader),
                           protocols_custom, nprotocols);

          XFree ((void *) protocols);
          g_free (protocols_custom);
        }

      gdk_window_add_filter (leader, exo_xsession_client_filter, client);
      g_object_ref (G_OBJECT (leader));
    }

  g_object_notify (G_OBJECT (client), "group");
}



/**
 * exo_xsession_client_get_group:
 * @client  : An #ExoXsessionClient.
 *
 * Return value: The client leader window of the group
 *               with which @client is associated or
 *               %NULL.
 **/
GdkWindow*
exo_xsession_client_get_group (ExoXsessionClient *client)
{
  g_return_val_if_fail (EXO_IS_XSESSION_CLIENT (client), NULL);
  return client->priv->leader;
}



/**
 * exo_xsession_client_get_restart_command:
 * @client  : An #ExoXsessionClient.
 * @argv    : Pointer to the location where the
 *            pointer to the argument vector should
 *            be stored to.
 * @argc    : Pointer to the location where the
 *            number of arguments should be stored
 *            to or %NULL.
 *
 * Retrieves the restart command previously set on @client. The
 * result is stored in @argv and should be freed using 
 * g_strfreev() when no longer needed.
 *
 * See exo_xsession_client_set_restart_command() for further
 * explanation.
 *
 * Return value: %TRUE on success, else %FALSE.
 **/
gboolean
exo_xsession_client_get_restart_command (ExoXsessionClient  *client,
                                         gchar            ***argv,
                                         gint               *argc)
{
  gchar **argv_return;
  gint    argc_return;

  g_return_val_if_fail (EXO_IS_XSESSION_CLIENT (client), FALSE);
  g_return_val_if_fail (argv != NULL, FALSE);

  if (G_UNLIKELY (client->priv->leader == NULL))
    {
      g_warning ("Tried to get the restart command for an ExoXsessionClient "
                 "instance, which is not connected to any client leader "
                 "window.");
      return FALSE;
    }

  if (XGetCommand (GDK_DRAWABLE_XDISPLAY (client->priv->leader),
                   GDK_DRAWABLE_XID (client->priv->leader),
                   &argv_return, &argc_return))
    {
      if (argc != NULL)
        *argc = argc_return;
      *argv = exo_strndupv (argv_return, argc_return);
      XFreeStringList (argv_return);
      return TRUE;
    }

  return FALSE;
}



/**
 * exo_xsession_client_set_restart_command:
 * @client  : An #ExoXsessionClient.
 * @argv    : The argument vector.
 * @argc    : The number of arguments in @argv.
 *
 * Sets the %WM_COMMAND property on the client leader window,
 * which instructs the session manager (or session-enabled window
 * manager) how to restart the application on next login.
 *
 * This function can only be used if @client is associated with
 * a client leader window.
 *
 * Please take note, that gtk_init() automatically sets the
 * %WM_COMMAND property on all client leader windows that are
 * implicitly created by Gtk+. So, you may only need to call
 * this function in response to the ::save-yourself signal.
 **/
void
exo_xsession_client_set_restart_command (ExoXsessionClient *client,
                                         gchar            **argv,
                                         gint               argc)
{
  g_return_if_fail (EXO_IS_XSESSION_CLIENT (client));
  g_return_if_fail (argv != NULL);
  g_return_if_fail (argc > 0);

  if (G_UNLIKELY (client->priv->leader == NULL))
    {
      g_warning ("Tried to set the restart command for an ExoXsessionClient "
                 "instance, which is not connected to any client leader "
                 "window.");
      return;
    }

  XSetCommand (GDK_DRAWABLE_XDISPLAY (client->priv->leader),
               GDK_DRAWABLE_XID (client->priv->leader),
               argv, argc);
}
