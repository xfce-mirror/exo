/* $Id: exo-file-watch.c,v 1.1.1.1 2004/09/14 22:32:58 bmeurer Exp $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo.h>



#define EXO_FILE_WATCH_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_FILE_WATCH, ExoFileWatchPrivate))



typedef struct _WatchEntity WatchEntity;
typedef struct _Watcher     Watcher;



static void     exo_file_watch_finalize      (GObject *object);
static gboolean exo_file_watch_timer         (gpointer data);
static void     exo_file_watch_timer_destroy (gpointer data);



struct _ExoFileWatchPrivate
{
  GSList  *entities;
  guint    watcher_id;
  guint    timer_id;
};

struct _WatchEntity
{
  GSList *watchers;
  gchar  *path;
  time_t  ctime;
  ino_t   inode;
};

struct _Watcher
{
  ExoFileWatchFunc func;
  gpointer          user_data;
  guint             id;
  gboolean          remove_this;
};



static GObjectClass   *parent_class;
static ExoFileWatch  *default_watch = NULL;



G_DEFINE_TYPE (ExoFileWatch, exo_file_watch, G_TYPE_OBJECT);



static void
exo_file_watch_class_init (ExoFileWatchClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoFileWatchPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_file_watch_finalize;
}



static void
exo_file_watch_init (ExoFileWatch *watch)
{
  watch->priv = EXO_FILE_WATCH_GET_PRIVATE (watch);
  watch->priv->watcher_id = 1;
}



static void
exo_file_watch_finalize (GObject *object)
{
  ExoFileWatch *watch = EXO_FILE_WATCH (object);
  WatchEntity   *entity;
  GSList        *ep;
  GSList        *wp;

  if (G_LIKELY (watch->priv->timer_id != 0))
    g_source_remove (watch->priv->timer_id);

#ifndef G_DISABLE_CHECKS
  {
    guint n = 0;
    for (ep = watch->priv->entities; ep != NULL; ep = ep->next)
      {
        entity = (WatchEntity *) ep->data;
        for (wp = entity->watchers; wp != NULL; wp = wp->next)
          if (!((Watcher *) wp->data)->remove_this)
            {
              ++n;
              break;
            }
      }

    if (n > 0)
      g_warning ("%u entities still alive when exo_file_watch_finalize was called", n);
  }
#endif

  for (ep = watch->priv->entities; ep != NULL; ep = ep->next)
    {
      entity = (WatchEntity *) ep->data;

      for (wp = entity->watchers; wp != NULL; wp = wp->next)
        g_free (wp->data);
      g_slist_free (entity->watchers);

      g_free (entity->path);
      g_free (entity);
    }
  g_slist_free (watch->priv->entities);

  parent_class->finalize (object);
}



static gboolean
exo_file_watch_timer (gpointer call_data)
{
  ExoFileWatchPrivate *priv = EXO_FILE_WATCH (call_data)->priv;
  ExoFileWatchAction   action;
  ExoFileWatch        *watch = EXO_FILE_WATCH (call_data);
  struct stat          sb;
  WatchEntity         *entity;
  gpointer             data;
  Watcher             *watcher;
  GSList              *next;
  GSList              *ep;
  GSList              *wp;

  g_return_val_if_fail (EXO_IS_FILE_WATCH (watch), TRUE);

  if (G_UNLIKELY (priv->entities == NULL))
    return FALSE;

  for (ep = priv->entities; ep != NULL; )
    {
      entity = (WatchEntity *) ep->data;
      if (lstat (entity->path, &sb) < 0)
        {
          if (errno == ENOENT || errno == ENOTDIR)
            {
              action = EXO_FILE_WATCH_DELETED;
              data = NULL;
            }
          else
            {
              action = EXO_FILE_WATCH_ERROR;
              data = GINT_TO_POINTER (errno);
            }
        }
      else if (sb.st_ino != entity->inode)
        {
          action = EXO_FILE_WATCH_DELETED;
          data = NULL;
        }
      else if (sb.st_ctime != entity->ctime)
        {
          action = EXO_FILE_WATCH_CHANGED;
          data = (gpointer) &sb;
          entity->ctime = sb.st_ctime;
        }
      else
        {
          ep = ep->next;
          continue;
        }

      for (wp = entity->watchers; wp != NULL; )
        {
          watcher = (Watcher *) wp->data;
          if (G_UNLIKELY (watcher->remove_this))
            {
              next = wp->next;
              entity->watchers = g_slist_delete_link (entity->watchers, wp);
              g_free (watcher);
              wp = next;
            }
          else
            {
              watcher->func (watch, action, entity->path,
                             data,  watcher->user_data);
              wp = wp->next;
            }
        }

      if (G_UNLIKELY (entity->watchers == NULL))
        {
          next = ep->next;
          priv->entities = g_slist_delete_link (priv->entities, ep);
          g_free (entity->path);
          g_free (entity);
          ep = next;
        }
      else 
        {
          ep = ep->next;
        }
    }

  return TRUE;
}



static void
exo_file_watch_timer_destroy (gpointer data)
{
  EXO_FILE_WATCH (data)->priv->timer_id = 0;
}



/**
 * exo_file_watch_get_default:
 *
 * Return value :
 **/
ExoFileWatch*
exo_file_watch_get_default (void)
{
  if (G_UNLIKELY (default_watch == NULL))
    {
      default_watch = g_object_new (EXO_TYPE_FILE_WATCH, NULL);
      g_object_add_weak_pointer (G_OBJECT (default_watch),
                                 (gpointer) &default_watch);
    }
  else
    {
      g_object_ref (G_OBJECT (default_watch));
    }

  return default_watch;
}



/**
 * exo_file_watch_add:
 * @watch       : A #ExoFileWatch.
 * @path        :
 * @func        :
 * @data        : pointer to a struct stat with information about @path if
 *                available.
 * @user_data   :
 * @error       :
 *
 * Return value :
 **/
guint
exo_file_watch_add (ExoFileWatch    *watch,
                     const gchar      *path,
                     ExoFileWatchFunc func,
                     gpointer          data,
                     gpointer          user_data,
                     GError          **error)
{
  ExoFileWatchPrivate *priv = watch->priv;
  struct stat         *stat;
  struct stat          sb;
  WatchEntity         *entity;
  Watcher             *watcher;
  GSList              *lp;

  g_return_val_if_fail (EXO_IS_FILE_WATCH (watch), 0);
  g_return_val_if_fail (path != NULL && *path != '\0', 0);
  g_return_val_if_fail (func != NULL, 0);

  watcher = g_new (Watcher, 1);
  watcher->func = func;
  watcher->user_data = user_data;
  watcher->id = priv->watcher_id++;
  watcher->remove_this = FALSE;

  for (lp = priv->entities; lp != NULL; lp = lp->next)
    {
      entity = (WatchEntity *) lp->data;
      if (strcmp (entity->path, path) == 0)
        {
          entity->watchers = g_slist_prepend (entity->watchers, watcher);
          return watcher->id;
        }
    }

  if (G_UNLIKELY (data == NULL))
    {
      if (lstat (path, &sb) < 0)
        {
          g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                       _("Unable to stat file %s"), path);
          g_free (watcher);
          return 0;
        }

      stat = &sb;
    }
  else
    {
      stat = (struct stat *) data;
    }

  entity = g_new (WatchEntity, 1);
  entity->path = g_strdup (path);
  entity->ctime = stat->st_ctime;
  entity->inode = stat->st_ino;
  entity->watchers = g_slist_append (NULL, watcher);
  priv->entities = g_slist_prepend (priv->entities, entity);

  if (G_UNLIKELY (priv->timer_id == 0))
    {
      priv->timer_id = g_timeout_add_full (G_PRIORITY_LOW,
                                           500,
                                           exo_file_watch_timer,
                                           watch,
                                           exo_file_watch_timer_destroy);
    }
                       

  return watcher->id;
}



/**
 * exo_file_watch_remove:
 * @watch :
 * @id    :
 **/
void
exo_file_watch_remove (ExoFileWatch *watch,
                       guint         id)
{
  ExoFileWatchPrivate *priv = watch->priv;
  Watcher             *watcher;
  GSList              *ep;
  GSList              *wp;

  g_return_if_fail (EXO_IS_FILE_WATCH (watch));
  g_return_if_fail (id > 0);

  for (ep = priv->entities; ep != NULL; ep = ep->next)
    for (wp = ((WatchEntity *) ep->data)->watchers; wp != NULL; wp = wp->next)
      {
        watcher = (Watcher *) wp->data;
        if (watcher->id == id)
          {
            watcher->remove_this = TRUE;
            return;
          }
      }

#ifndef G_DISABLE_CHECKS
  g_warning ("No such watcher of id %u", id);
#endif
}


