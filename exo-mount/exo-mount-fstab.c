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

#ifdef HAVE_SYS_MNTTAB_H
#include <sys/mnttab.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_FSTAB_H
#include <fstab.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo-mount/exo-mount-fstab.h>
#include <exo-mount/exo-mount-utils.h>



/* define _PATH_FSTAB if undefined */
#ifndef _PATH_FSTAB
#ifdef sun
#define _PATH_FSTAB "/etc/vfstab"
#else
#define _PATH_FSTAB "/etc/fstab"
#endif
#endif

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
  gchar *path = NULL;

#if defined(HAVE_SETMNTENT) /* Linux */
  struct mntent *mntent;
  gchar         *resolved;
  FILE          *fp;

  /* try to open the fstab file */
  fp = setmntent (_PATH_FSTAB, "r");
  if (G_UNLIKELY (fp == NULL))
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Failed to open file \"%s\": %s"), _PATH_FSTAB,
                   g_strerror (errno));
      return NULL;
    }

  /* look up an entry for our device file */
  while (path == NULL)
    {
      /* grab the next entry */
      mntent = getmntent (fp);
      if (mntent == NULL)
        break;

      /* check if this entry matches */
      if (strcmp (device_file, mntent->mnt_fsname) == 0)
        {
          /* exakt match, nice */
          path = g_strdup (mntent->mnt_dir);
        }
      else
        {
          /* but maybe the fstab entry is a symlink */
          resolved = exo_mount_utils_resolve (mntent->mnt_fsname);
          if (strcmp (device_file, resolved) == 0)
            path = g_strdup (mntent->mnt_dir);
          g_free (resolved);
        }
    }

  /* close the file handle */
  endmntent (fp);
#elif defined(HAVE_GETMNTENT) /* Solaris */
  struct mnttab mntent;
  gchar        *resolved;
  FILE         *fp;

  /* try to open the fstab file */
  fp = fopen (_PATH_FSTAB, "r");
  if (G_UNLIKELY (fp == NULL))
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Failed to open file \"%s\": %s"), _PATH_FSTAB,
                   g_strerror (errno));
      return NULL;
    }

  /* look up an entry for our device file */
  while (path == NULL)
    {
      /* grab the next entry */
      if (getmntent (fp, &mntent) != 0)
        break;

      /* check if this entry matches */
      if (strcmp (device_file, mntent.mnt_special) == 0)
        {
          /* exakt match, nice */
          path = g_strdup (mntent.mnt_mountp);
        }
      else
        {
          /* but maybe the fstab entry is a symlink */
          resolved = exo_mount_utils_resolve (mntent.mnt_special);
          if (strcmp (device_file, resolved) == 0)
            path = g_strdup (mntent.mnt_mountp);
          g_free (resolved);
        }
    }

  /* close the file handle */
  fclose (fp);
#elif defined(HAVE_GETFSSPEC) /* FreeBSD */
  struct fstab *fs;

  /* look up the entry for the device file,
   * fortunately FreeBSD isn't playing the
   * weird symlink tricks for most devices,
   * so we don't need the damn stupid Linux
   * symlink resolving stuff here...
   */
  fs = getfsspec (device_file);
  if (G_LIKELY (fs != NULL))
    {
      /* check if this is a usable file system */
      if (strcmp (fs->fs_type, FSTAB_SW) != 0
#ifdef FSTAB_DP
          && strcmp (fs->fs_type, FSTAB_DP) != 0
#endif
          && strcmp (fs->fs_type, FSTAB_XX) != 0)
        {
          /* jap, usable file system */
          path = g_strdup (fs->fs_file);
        }
    }
#else
#error "Add support for your operating system here."
#endif

  /* check if we failed to find the entry */
  if (G_UNLIKELY (path == NULL))
    {
      /* generate an appropriate error message */
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Device \"%s\" not found in file system device table"), device_file);
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
