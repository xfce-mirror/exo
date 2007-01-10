/* $Id$ */
/*-
 * Copyright (c) 2006-2007 Benedikt Meurer <benny@xfce.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo-mount/exo-mount-utils.h>



/**
 * exo_mount_utils_is_mounted:
 * @device_file     : an absolute path to a device file.
 * @readonly_return : if non-%NULL and the device is mounted, this
 *                    specifies whether the device was mounted ro.
 *
 * Returns %TRUE if the @device_file is already mounted
 * somewhere in the system, %FALSE otherwise.
 *
 * Return value: %TRUE if @device_file is mounted, else %FALSE.
 **/
gboolean
exo_mount_utils_is_mounted (const gchar *device_file,
                            gboolean    *readonly_return)
{
  GSList *mount_points;

  /* check if we have an active mount point for the device file */
  mount_points = exo_mount_point_list_matched (EXO_MOUNT_POINT_MATCH_ACTIVE | EXO_MOUNT_POINT_MATCH_DEVICE, device_file, NULL, NULL, NULL);
  if (G_LIKELY (mount_points != NULL))
    {
      /* check if the first matching device is mounted read-only */
      if (G_LIKELY (readonly_return != NULL))
        *readonly_return = ((((const ExoMountPoint *) mount_points->data)->flags & EXO_MOUNT_POINT_READ_ONLY) != 0);
      g_slist_foreach (mount_points, (GFunc) exo_mount_point_free, NULL);
      g_slist_free (mount_points);
      return TRUE;
    }

  return FALSE;
}

