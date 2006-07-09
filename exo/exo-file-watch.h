/* $Id$ */
/*-
 * Copyright (c) 2004  Benedikt Meurer <benny@xfce.org>
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

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EXO_FILE_WATCH_H__
#define __EXO_FILE_WATCH_H__

#include <glib-object.h>

G_BEGIN_DECLS;

#define EXO_TYPE_FILE_WATCH            (exo_file_watch_get_type ())
#define EXO_FILE_WATCH(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_FILE_WATCH, ExoFileWatch))
#define EXO_FILE_WATCH_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_FILE_WATCH, ExoFileWatchClass))
#define EXO_IS_FILE_WATCH(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_FILE_WATCH))
#define EXO_IS_FILE_WATCH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_FILE_WATCH))
#define EXO_FILE_WATCH_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_FILE_WATCH, ExoFileWatchClass))

typedef struct _ExoFileWatchPrivate ExoFileWatchPrivate;
typedef enum   _ExoFileWatchAction  ExoFileWatchAction;
typedef struct _ExoFileWatchClass   ExoFileWatchClass;
typedef struct _ExoFileWatch        ExoFileWatch;

enum _ExoFileWatchAction
{
  EXO_FILE_WATCH_CHANGED,
  EXO_FILE_WATCH_DELETED,
  EXO_FILE_WATCH_ERROR,
};

struct _ExoFileWatchClass
{
  GObjectClass  __parent__;
};

struct _ExoFileWatch
{
  GObject              __parent__;
  ExoFileWatchPrivate *priv;
};

typedef void (*ExoFileWatchFunc) (ExoFileWatch       *watch,
                                  ExoFileWatchAction  action,
                                  const gchar        *path,
                                  gpointer            data,
                                  gpointer            user_data);

GType         exo_file_watch_get_type    (void) G_GNUC_CONST;

ExoFileWatch *exo_file_watch_get_default (void);

guint         exo_file_watch_add         (ExoFileWatch    *watch,
                                          const gchar     *path,
                                          ExoFileWatchFunc func,
                                          gpointer         data,
                                          gpointer         user_data,
                                          GError         **error);

void          exo_file_watch_remove      (ExoFileWatch    *watch,
                                          guint            id);

G_END_DECLS;

#endif /* !__EXO_FILE_WATCH_H__ */
