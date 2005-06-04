/* $Id$ */
/*-
 * Copyright (c) 2005 os-cillation e.K.
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

#include <exo/exo-mime-database.h>

#include <xdgmime/xdgmime.h>



enum
{
  CHANGED,
  LAST_SIGNAL,
};



static void     exo_mime_database_class_init          (ExoMimeDatabaseClass *klass);
static void     exo_mime_database_init                (ExoMimeDatabase      *database);
static void     exo_mime_database_finalize            (GObject              *object);
static void     exo_mime_database_reload              (gpointer              user_data);
static gboolean exo_mime_database_reload_idle         (gpointer              user_data);
static void     exo_mime_database_reload_idle_destroy (gpointer              user_data);



struct _ExoMimeDatabaseClass
{
  GObjectClass __parent__;

  /* signals */
  void (*changed) (ExoMimeDatabase *database);

  void (*reserved1) (void);
  void (*reserved2) (void);
  void (*reserved3) (void);
  void (*reserved4) (void);
  void (*reserved5) (void);
  void (*reserved6) (void);
  void (*reserved7) (void);
  void (*reserved8) (void);
};

struct _ExoMimeDatabase
{
  GObject __parent__;

  GHashTable *info_cache;
  gint        reload_idle_id;
  gint        reload_id;
};



static GObjectClass *parent_class;
static guint         database_signals[LAST_SIGNAL];



G_DEFINE_TYPE (ExoMimeDatabase, exo_mime_database, G_TYPE_OBJECT);



static void
exo_mime_database_class_init (ExoMimeDatabaseClass *klass)
{
  GObjectClass *gobject_class;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_mime_database_finalize;

  /**
   * ExoMimeDatabase::changed:
   * @database  : An #ExoMimeDatabase instance.
   *
   * This signal is emitted whenever the underlying implementation
   * notices a changed to the on-disk representation of the MIME
   * database.
   *
   * The modules using the MIME framework contained in libexo
   * should connect a slot to this signal to be informed of
   * changes to the MIME database, and perform the appropriate
   * actions (e.g. recalculating the MIME types for all files
   * currently displayed) once this signal is emitted.
   **/
  database_signals[CHANGED] =
    g_signal_new ("changed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (ExoMimeDatabaseClass, changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}



static void
exo_mime_database_init (ExoMimeDatabase *database)
{
  database->reload_idle_id = -1;

  /* register the reload notification with the XDG mime implementation */
  database->reload_id =
    xdg_mime_register_reload_callback (exo_mime_database_reload,
                                       database, NULL);

  /* allocate the internal MIME info cache */
  database->info_cache =
    g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
}



static void
exo_mime_database_finalize (GObject *object)
{
  ExoMimeDatabase *database = EXO_MIME_DATABASE (object);

  /* unregister the main loop idle callback (if any) */
  if (database->reload_idle_id >= 0)
    g_source_remove (database->reload_idle_id);

  /* drop the internal MIME info cache */
  g_hash_table_destroy (database->info_cache);

  /* unregister the internal reload notification */
  xdg_mime_remove_callback (database->reload_id);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}



static void
exo_mime_database_reload (void *user_data)
{
  ExoMimeDatabase *database = EXO_MIME_DATABASE (user_data);

  /* We use the idle callback approach here, as the xdgmime
   * reload callback will be called from within an ordinary
   * xdg_mime_ function invokation and so handlers would need
   * to be reentrant, which makes things unnecessary complex.
   */
  if (database->reload_idle_id < 0)
    {
      database->reload_idle_id =
        g_idle_add_full (G_PRIORITY_HIGH, exo_mime_database_reload_idle,
                         database, exo_mime_database_reload_idle_destroy);
    }
}



static gboolean
exo_mime_database_reload_idle (void *user_data)
{
  ExoMimeDatabase *database = EXO_MIME_DATABASE (user_data);

  GDK_THREADS_ENTER ();

  /* drop all cached MIME info instances */
  g_hash_table_foreach_remove (database->info_cache,
                               (GHRFunc) gtk_true, NULL);

  g_signal_emit (G_OBJECT (database), database_signals[CHANGED], 0);

  GDK_THREADS_LEAVE ();

  return FALSE;
}



static void
exo_mime_database_reload_idle_destroy (void *user_data)
{
  GDK_THREADS_ENTER ();

  EXO_MIME_DATABASE (user_data)->reload_idle_id = -1;

  GDK_THREADS_LEAVE ();
}



/**
 * exo_mime_database_get_default:
 *
 * Returns the default instance of the #ExoMimeDatabase class. This
 * instance is shared by all modules using this class. You may
 * also create a new instance of the #ExoMimeDatabase class using
 * the GObject mechanism for instantiating classes, but the
 * recommended way is to use the shared instance instead.
 *
 * Return value: The shared instance of the #ExoMimeDatabase class.
 **/
ExoMimeDatabase*
exo_mime_database_get_default (void)
{
  static ExoMimeDatabase* database = NULL;

  if (G_UNLIKELY (database == NULL))
    {
      database = g_object_new (EXO_TYPE_MIME_DATABASE, NULL);
      g_object_add_weak_pointer (G_OBJECT (database),
                                 (gpointer) &database);
    }
  else
    {
      g_object_ref (G_OBJECT (database));
    }

  return database;
}



/**
 * exo_mime_database_get_info:
 * @database  : An #ExoMimeDatabase instance.
 * @name      : The name of the MIME type to query.
 *
 * Looks up the MIME info for the given @name in @database and
 * returns an instance of the #ExoMimeInfo class describing the
 * MIME type in detail.
 *
 * Call #g_object_unref on the returned #ExoMimeInfo instance
 * if you don't need it any longer.
 *
 * Return value: An #ExoMimeInfo instance describing the MIME
 *               type of the given @name.
 **/
ExoMimeInfo*
exo_mime_database_get_info (ExoMimeDatabase *database,
                            const gchar     *name)
{
  ExoMimeInfo *info;

  g_return_val_if_fail (EXO_IS_MIME_DATABASE (database), NULL);
  g_return_val_if_fail (name != 0 && *name != '\0', NULL);

  info = g_hash_table_lookup (database->info_cache, name);
  if (G_UNLIKELY (info == NULL))
    {
      info = g_object_new (EXO_TYPE_MIME_INFO,
                           "name", name,
                           NULL);

      /* We don't take another copy of the name here, as we can be sure
       * that the name of the ExoMimeInfo instance won't be modified
       * while we keep a reference on it.
       */
      name = exo_mime_info_get_name (info);
      g_hash_table_insert (database->info_cache, (gpointer)name, info);
    }
  
  g_object_ref (G_OBJECT (info));

  return info;
}



/**
 * exo_mime_database_get_info_for_data:
 * @database  : An #ExoMimeDatabase instance.
 * @data      : The buffer containing the data.
 * @length    : The length of @data in bytes.
 *
 * Call #g_object_unref on the returned #ExoMimeInfo instance
 * if you don't need it any longer.
 *
 * Return value: The appropriate MIME info for the given @data.
 **/
ExoMimeInfo*
exo_mime_database_get_info_for_data (ExoMimeDatabase *database,
                                     gconstpointer    data,
                                     gsize            length)
{
  const gchar *name;

  g_return_val_if_fail (EXO_IS_MIME_DATABASE (database), NULL);
  g_return_val_if_fail (data != 0, NULL);
  g_return_val_if_fail (length > 0, NULL);

  name = xdg_mime_get_mime_type_for_data (data, length);
  return exo_mime_database_get_info (database, name);
}



/**
 * exo_mime_database_get_info_for_file:
 * @database  : An #ExoMimeDatabase instance.
 * @file_name : The path to the file whose MIME type should be determined.
 *
 * Call #g_object_unref on the returned #ExoMimeInfo instance
 * if you don't need it any longer.
 *
 * Return value: The appropriate MIME info for the given file.
 **/
ExoMimeInfo*
exo_mime_database_get_info_for_file (ExoMimeDatabase *database,
                                     const gchar     *file_name)
{
  const gchar *name;

  g_return_val_if_fail (EXO_IS_MIME_DATABASE (database), NULL);
  g_return_val_if_fail (file_name != 0, NULL);

  name = xdg_mime_get_mime_type_for_file (file_name);
  return exo_mime_database_get_info (database, name);
}



/**
 * exo_mime_database_get_info_from_file_name:
 * @database  : An #ExoMimeDatabase instance.
 * @file_name : The path to the file whose MIME type should be determined.
 *
 * Call #g_object_unref on the returned #ExoMimeInfo instance
 * if you don't need it any longer.
 *
 * Return value: The appropriate MIME info for the given file name.
 **/
ExoMimeInfo*
exo_mime_database_get_info_from_file_name (ExoMimeDatabase *database,
                                           const gchar     *file_name)
{
  const gchar *name;

  g_return_val_if_fail (EXO_IS_MIME_DATABASE (database), NULL);
  g_return_val_if_fail (file_name != 0, NULL);

  name = xdg_mime_get_mime_type_from_file_name (file_name);
  return exo_mime_database_get_info (database, name);
}


