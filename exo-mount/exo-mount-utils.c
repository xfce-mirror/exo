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

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_UCRED_H
#include <sys/ucred.h>
#endif
#ifdef HAVE_SYS_MNTTAB_H
#include <sys/mnttab.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo-mount/exo-mount-utils.h>



static void exo_mount_utils_canonicalize_filename (gchar *filename);



/* borrowed from gtk/gtkfilesystemunix.c in GTK+ on 02/23/2006 */
static void
exo_mount_utils_canonicalize_filename (gchar *filename)
{
  gboolean last_was_slash = FALSE;
  gchar   *p = filename;
  gchar   *q = filename;

  while (*p)
    {
      if (*p == G_DIR_SEPARATOR)
        {
          if (!last_was_slash)
            *q++ = G_DIR_SEPARATOR;
          last_was_slash = TRUE;
        }
      else
        {
          if (last_was_slash && *p == '.')
            {
              if (*(p + 1) == G_DIR_SEPARATOR || *(p + 1) == '\0')
              {
                if (*(p + 1) == '\0')
                  break;
                
                p += 1;
              }
              else if (*(p + 1) == '.' && (*(p + 2) == G_DIR_SEPARATOR || *(p + 2) == '\0'))
                {
                  if (q > filename + 1)
                    {
                      q--;
                      while (q > filename + 1 && *(q - 1) != G_DIR_SEPARATOR)
                        q--;
                    }
                  
                  if (*(p + 2) == '\0')
                    break;
                  
                  p += 2;
                }
              else
                {
                  *q++ = *p;
                  last_was_slash = FALSE;
                }
            }
          else
            {
              *q++ = *p;
              last_was_slash = FALSE;
            }
        }
      
      p++;
    }
  
  if (q > filename + 1 && *(q - 1) == G_DIR_SEPARATOR)
    q--;
  
  *q = '\0';
}



/**
 * exo_mount_utils_resolve:
 * @device_file     : absolute or relative path to a device file.
 *
 * Resolves the @device_file path, which might be a symbolic link
 * to it's real path. The caller is responsible to free the returned
 * string using g_free() when no longer needed.
 *
 * Return value: the resolved path of the @device_file.
 **/
gchar*
exo_mount_utils_resolve (const gchar *device_file)
{
  gchar *dirname;
  gchar *target;
  gchar *path;

  g_return_val_if_fail (device_file != NULL, NULL);

  /* check if it's an absolute path */
  if (!g_path_is_absolute (device_file))
    path = g_build_filename ("/dev", device_file, NULL);
  else
    path = g_strdup (device_file);

  /* resolve all present symlinks */
  while (g_file_test (path, G_FILE_TEST_IS_SYMLINK))
    {
      /* use the path, if we cannot resolve a symlink */
      target = g_file_read_link (path, NULL);
      if (G_UNLIKELY (target == NULL))
        break;

      /* check if the link is relative */
      if (!g_path_is_absolute (target))
        {
          /* relative to the current path then */
          dirname = g_path_get_dirname (path);
          g_free (path);
          path = g_build_filename (dirname, target, NULL);
          g_free (dirname);
          g_free (target);
        }
      else
        {
          /* use the absolute target */
          g_free (path);
          path = target;
        }
    }

  /* canonicalize the path */
  exo_mount_utils_canonicalize_filename (path);

  return path;
}



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
  gboolean result = FALSE;
  gchar   *resolved;

#if defined(HAVE_SETMNTENT) /* Linux */
  struct mntent *mntent;
  FILE          *fp;

  /* try to open the /proc/mounts file */
  fp = setmntent ("/proc/mounts", "r");
  if (G_LIKELY (fp != NULL))
    {
      /* process all mnt entries */
      while (!result)
        {
          /* read the next entry */
          mntent = getmntent (fp);
          if (mntent == NULL)
            break;

          /* check if this is the entry we are looking for */
          result = (strcmp (mntent->mnt_fsname, device_file) == 0);
          if (G_LIKELY (!result))
            {
              /* but maybe the mount entry is a symlink */
              resolved = exo_mount_utils_resolve (mntent->mnt_fsname);
              result = (strcmp (resolved, device_file) == 0);
              g_free (resolved);
            }

          /* check if the device was mounted read-only */
          if (readonly_return != NULL && result)
            *readonly_return = (hasmntopt (mntent, "ro") != NULL);
        }

      /* close the file handle */
      endmntent (fp);
    }
#elif defined(HAVE_GETMNTENT) /* Solaris */
  struct mnttab mntent;
  FILE         *fp;

  /* try to open the /proc/mountfs file */
  fp = fopen ("/proc/mounts", "r");
  if (G_LIKELY (fp != NULL))
    {
      /* process all mnt entries */
      while (!result)
        {
          /* grab the next entry */
          if (getmntent (fp, &mntent) != 0)
            break;

          /* check if this is the entry we are looking for */
          result = (strcmp (mntent.mnt_special, device_file) == 0);
          if (G_LIKELY (!result))
            {
              /* but maybe the mount entry is a symlink */
              resolved = exo_mount_utils_resolve (mntent.mnt_special);
              result = (strcmp (resolved, device_file) == 0);
              g_free (resolved);
            }

          /* check if the device was mounted read-only */
          if (readonly_return != NULL && result)
            *readonly_return = (hasmntopt (&mntent, "ro") != NULL);
        }

      /* close the file handle */
      fclose (fp);
    }
#elif defined(HAVE_GETFSSTAT) /* FreeBSD */
  struct statfs *mntbuf = NULL;
  glong          bufsize = 0;
  gint           mntsize;
  gint           n;

  /* determine the number of active mount points */
  mntsize = getfsstat (NULL, 0, MNT_NOWAIT);
  if (G_LIKELY (mntsize > 0))
    {
      /* allocate a new buffer */
      bufsize = (mntsize + 4) * sizeof (*mntbuf);
      mntbuf = (struct statfs *) g_malloc (bufsize);

      /* determine the mount point for the device file */
      mntsize = getfsstat (mntbuf, bufsize, MNT_NOWAIT);
      for (n = 0; n < mntsize && !result; ++n)
        {
          /* check if this is the entry we are looking for */
          result = (strcmp (mntbuf[n].f_mntfromname, device_file) == 0);
          if (G_LIKELY (!result))
            {
              /* but maybe the mount entry is a symlink */
              resolved = exo_mount_utils_resolve (mntbuf[n].f_mntfromname);
              result = (strcmp (resolved, device_file) == 0);
              g_free (resolved);
            }

          /* check if the device was mounted read-only */
          if (readonly_return != NULL && result)
            *readonly_return = ((mntbuf[n].f_flags & MNT_RDONLY) != 0);
        }

      /* release the buffer */
      g_free (mntbuf);
    }
#else
#error "Add support for your operating system here."
#endif

  return result;
}


