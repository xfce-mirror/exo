/* $Id$ */
/*-
 * Copyright (c) 2005-2007 Benedikt Meurer <benny@xfce.org>.
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

#ifndef __EXO_MOUNT_POINT_H__
#define __EXO_MOUNT_POINT_H__

#include <exo/exo-config.h>

G_BEGIN_DECLS;

/**
 * ExoMountPointFlags:
 * @EXO_MOUNT_POINT_READ_ONLY : read only mount point.
 *
 * Identifies options of #ExoMountPoint<!---->s.
 *
 * Since: 0.3.1.13
 **/
typedef enum /*< flags >*/
{
  EXO_MOUNT_POINT_READ_ONLY = (1L << 0),
} ExoMountPointFlags;

#define EXO_TYPE_MOUNT_POINT (exo_mount_point_get_type ())

typedef struct _ExoMountPoint ExoMountPoint;
struct _ExoMountPoint
{
  ExoMountPointFlags flags;
  gchar             *device;
  gchar             *folder;
  gchar             *fstype;
};

/**
 * ExoMountPointMatchMask:
 * @EXO_MOUNT_POINT_MATCH_ACTIVE     : see exo_mount_point_list_active().
 * @EXO_MOUNT_POINT_MATCH_CONFIGURED : see exo_mount_point_list_configured().
 * @EXO_MOUNT_POINT_MATCH_DEVICE     : match by device file.
 * @EXO_MOUNT_POINT_MATCH_FOLDER     : match by mount point folder.
 * @EXO_MOUNT_POINT_MATCH_FSTYPE     : match by file system type.
 *
 * Flags for exo_mount_point_list_matched(), that control which mount points
 * will be returned. The fewer match options are specified, the more mount
 * points will usually match (surprising, eh?).
 *
 * Since: 0.3.1.13
 **/
typedef enum /*< skip >*/
{
  EXO_MOUNT_POINT_MATCH_ACTIVE      = (0L << 0),
  EXO_MOUNT_POINT_MATCH_CONFIGURED  = (1L << 0),
  EXO_MOUNT_POINT_MATCH_DEVICE      = (1L << 1),
  EXO_MOUNT_POINT_MATCH_FOLDER      = (1L << 2),
  EXO_MOUNT_POINT_MATCH_FSTYPE      = (1L << 3),
} ExoMountPointMatchMask;

GType          exo_mount_point_get_type     (void) G_GNUC_CONST;

ExoMountPoint *exo_mount_point_dup          (const ExoMountPoint *mount_point) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
void           exo_mount_point_free         (ExoMountPoint       *mount_point);

GSList        *exo_mount_point_list_matched (ExoMountPointMatchMask mask,
                                             const gchar           *device,
                                             const gchar           *folder,
                                             const gchar           *fstype,
                                             GError               **error) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

/**
 * exo_mount_point_list_active:
 * @error : return location for errors or %NULL.
 *
 * Convenience wrapper for exo_mount_point_list_matched(), that returns the
 * currently active mount points, or %NULL in case of an error.
 *
 * The caller is responsible to free the returned list using
 * <informalexample><programlisting>
 * g_slist_foreach (list, (GFunc) exo_mount_point_free, NULL);
 * g_slist_free (list);
 * </programlisting></informalexample>
 * when no longer needed.
 *
 * Return value: the list of currently active mount points.
 *
 * Since: 0.3.1.13
 **/
#define exo_mount_point_list_active(error) (exo_mount_point_list_matched (EXO_MOUNT_POINT_MATCH_ACTIVE, NULL, NULL, NULL, (error)))

/**
 * exo_mount_point_list_configured:
 * @error : return location for errors or %NULL.
 *
 * Convenience wrapper for exo_mount_point_list_matched(), that returns the
 * configured mount points, i.e. the entries from the file system table (which
 * is usually specified in <filename>/etc/fstab</filename>).
 *
 * The caller is responsible to free the returned list using
 * <informalexample><programlisting>
 * g_slist_foreach (list, (GFunc) exo_mount_point_free, NULL);
 * g_slist_free (list);
 * </programlisting></informalexample>
 * when no longer needed.
 *
 * Return value: the list of configured mount points.
 *
 * Since: 0.3.1.13
 **/
#define exo_mount_point_list_configured(error) (exo_mount_point_list_matched (EXO_MOUNT_POINT_MATCH_CONFIGURED, NULL, NULL, NULL, (error)))

G_END_DECLS;

#endif /* !__EXO_MOUNT_POINT_H__ */
