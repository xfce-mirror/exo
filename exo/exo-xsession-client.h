/*-
 * Copyright (c) 2004 os-cillation e.K.
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EXO_XSESSION_CLIENT_H__
#define __EXO_XSESSION_CLIENT_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS

typedef struct _ExoXsessionClientPrivate ExoXsessionClientPrivate;
typedef struct _ExoXsessionClientClass   ExoXsessionClientClass;
typedef struct _ExoXsessionClient        ExoXsessionClient;

#define EXO_TYPE_XSESSION_CLIENT            (exo_xsession_client_get_type ())
#define EXO_XSESSION_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_XSESSION_CLIENT, ExoXsessionClient))
#define EXO_XSESSION_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_XSESSION_CLIENT, ExoXsessionClientClass))
#define EXO_IS_XSESSION_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_XSESSION_CLIENT))
#define EXO_IS_XSESSION_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_XSESSION_CLIENT))
#define EXO_XSESSION_CLIENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_XSESSION_CLIENT, ExoXsessionClientClass))

struct _ExoXsessionClientClass
{
  GObjectClass __parent__;

  /* signals */
  void (*save_yourself) (ExoXsessionClient *client);

  void (*reserved1) (void);
  void (*reserved2) (void);
};

/**
 * ExoXsessionClient:
 *
 * The ExoXsessionClient struct contains only private fields and should
 * not be directly accessed.
 **/
struct _ExoXsessionClient
{
  GObject __parent__;

  /*< private >*/
  ExoXsessionClientPrivate *priv;
};

GType              exo_xsession_client_get_type             (void) G_GNUC_CONST;

ExoXsessionClient *exo_xsession_client_new_with_group       (GdkWindow          *leader);

GdkWindow         *exo_xsession_client_get_group            (ExoXsessionClient  *client);
void               exo_xsession_client_set_group            (ExoXsessionClient  *client,
                                                             GdkWindow          *leader);

gboolean           exo_xsession_client_get_restart_command  (ExoXsessionClient  *client,
                                                             gchar            ***argv,
                                                             gint               *argc);
void               exo_xsession_client_set_restart_command  (ExoXsessionClient  *client,
                                                             gchar             **argv,
                                                             gint                argc);

G_END_DECLS

#endif /* !__EXO_XSESSION_CLIENT_H__ */
