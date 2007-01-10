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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo-mount/exo-mount-fstab.h>
#include <exo-mount/exo-mount-utils.h>



/* define _PATH_MOUNT if undefined */
#ifndef _PATH_MOUNT
#define _PATH_MOUNT "/bin/mount"
#endif



static gboolean exo_mount_fstab_exec   (const gchar *command,
                                        const gchar *argument,
                                        GError     **error);
static gchar   *exo_mount_fstab_lookup (const gchar *device_file,
                                        GError     **error) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;



static gboolean
exo_mount_fstab_exec (const gchar *command,
                      const gchar *argument,
                      GError     **error)
{
  gboolean result;
  gchar   *standard_error;
  gchar   *command_line;
  gchar   *quoted;
  gint     status;

  g_return_val_if_fail (command != NULL, FALSE);
  g_return_val_if_fail (argument != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* generate the command line */
  quoted = g_shell_quote (argument);
  command_line = g_strconcat (command, " ", quoted, NULL);
  g_free (quoted);

  /* try to execute the command line */
  result = g_spawn_command_line_sync (command_line, NULL, &standard_error, &status, error);
  if (G_LIKELY (result))
    {
      /* check if the command failed */
      if (G_UNLIKELY (status != 0))
        {
          /* drop additional whitespace from the stderr output */
          g_strstrip (standard_error);

          /* strip all trailing dots from the stderr output */
          while (*standard_error != '\0' && standard_error[strlen (standard_error) - 1] == '.')
            standard_error[strlen (standard_error) - 1] = '\0';

          /* generate an error from the stderr output */
          if (G_LIKELY (*standard_error != '\0'))
            g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, "%s", standard_error);
          else
            g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Unknown error"));

          /* and yes, we failed */
          result = FALSE;
        }

      /* release the stderr output */
      g_free (standard_error);
    }

  /* cleanup */
  g_free (command_line);

  return result;
}



static gchar*
exo_mount_fstab_lookup (const gchar *device_file,
                        GError     **error)
{
  GError *err = NULL;
  GSList *mount_points;
  gchar  *path = NULL;

  /* lookup the configured device in the file system table using the ExoMountPoint module */
  mount_points = exo_mount_point_list_matched (EXO_MOUNT_POINT_MATCH_CONFIGURED | EXO_MOUNT_POINT_MATCH_DEVICE, device_file, NULL, NULL, &err);
  if (G_LIKELY (mount_points != NULL))
    {
      /* take a copy of the folder path of the first matching mount point */
      path = g_strdup (((const ExoMountPoint *) mount_points->data)->folder);

      /* cleanup the mount points */
      g_slist_foreach (mount_points, (GFunc) exo_mount_point_free, NULL);
      g_slist_free (mount_points);
    }
  else if (err == NULL)
    {
      /* TRANSLATORS: a device is missing from the file system table (usually /etc/fstab) */
      g_set_error (&err, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Device \"%s\" not found in file system device table"), device_file);
    }

  /* check if we failed */
  if (G_UNLIKELY (err != NULL))
    {
      /* propagate the error */
      g_propagate_error (error, err);
      return NULL;
    }

  return path;
}



/**
 * exo_mount_fstab_contains:
 * @device_file : the absolute path to a block device file.
 *
 * Checks whether an entry for the @device_file exists in the
 * file system table file <tt>/etc/fstab</tt>. Returns %TRUE if
 * such an entry exists, %FALSE otherwise.
 *
 * Return value: %TRUE if an entry for @device_file is present
 *               in <tt>/dev/fstab</tt>, %FALSE otherwise.
 **/
gboolean
exo_mount_fstab_contains (const gchar *device_file)
{
  gchar *mount_point;

  g_return_val_if_fail (g_path_is_absolute (device_file), FALSE);

  /* check if we have an fstab entry */
  mount_point = exo_mount_fstab_lookup (device_file, NULL);
  if (G_LIKELY (mount_point != NULL))
    {
      /* jap, match found */
      g_free (mount_point);
      return TRUE;
    }

  /* no match */
  return FALSE;
}



/**
 * exo_mount_fstab_eject:
 * @device_file : the absolute path to a block device file.
 * @error       : return location for errors or %NULL.
 *
 * Ejects the device identified by the @device_file. Returns
 * %TRUE if the device was successfully ejected, %FALSE otherwise.
 *
 * Return value: %TRUE on success, %FALSE if @error is set.
 **/
gboolean
exo_mount_fstab_eject (const gchar *device_file,
                       GError     **error)
{
  gboolean result;
  gchar   *mount_point;
  gchar   *device_name;

  g_return_val_if_fail (g_path_is_absolute (device_file), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* verify that the device is listed in the fstab */
  mount_point = exo_mount_fstab_lookup (device_file, error);
  if (G_UNLIKELY (mount_point == NULL))
    return FALSE;
  g_free (mount_point);

  /* try to eject the device */
  device_name = g_path_get_basename (device_file);
  result = exo_mount_fstab_exec ("eject", device_name, error);
  g_free (device_name);

  return result;
}



/**
 * exo_mount_fstab_mount:
 * @device_file : the absolute path to a block device file.
 * @error       : return location for errors or %NULL.
 *
 * Mounts the device identified by the @device_file. Returns %TRUE
 * if the device was successfully mounted, %FALSE in case or an
 * error.
 *
 * Return value: %TRUE if successfull, %FALSE otherwise.
 **/
gboolean
exo_mount_fstab_mount (const gchar *device_file,
                       GError     **error)
{
  gboolean result;
  gchar   *mount_point;

  g_return_val_if_fail (g_path_is_absolute (device_file), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* determine the mount point of the device from the fstab */
  mount_point = exo_mount_fstab_lookup (device_file, error);
  if (G_UNLIKELY (mount_point == NULL))
    return FALSE;

  /* try to mount the device */
  result = exo_mount_fstab_exec (_PATH_MOUNT, mount_point, error);

  /* cleanup */
  g_free (mount_point);

  return result;
}




/**
 * exo_mount_fstab_unmount:
 * @device_file : the absolute path to a block device file.
 * @error       : return location for errors or %NULL.
 *
 * Unmounts the device identified by the @device_file. Returns
 * %TRUE if the device was successfully unmounted, %FALSE in
 * case of an error.
 *
 * Return value: %TRUE if successfull, %FALSE otherwise.
 **/
gboolean
exo_mount_fstab_unmount (const gchar *device_file,
                         GError     **error)
{
  gboolean result;
  gchar   *mount_point;
  gchar   *dirname;
  gchar   *command;

  g_return_val_if_fail (g_path_is_absolute (device_file), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* determine the mount point of the device from the fstab */
  mount_point = exo_mount_fstab_lookup (device_file, error);
  if (G_UNLIKELY (mount_point == NULL))
    return FALSE;

  /* determine umount from _PATH_MOUNT */
  dirname = g_path_get_dirname (_PATH_MOUNT);
  command = g_build_filename (dirname, "umount", NULL);
  g_free (dirname);

  /* try to mount the device */
  result = exo_mount_fstab_exec (command, mount_point, error);

  /* cleanup */
  g_free (mount_point);
  g_free (command);

  return result;
}
