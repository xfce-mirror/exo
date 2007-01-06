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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
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
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
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



/**
 * exo_mount_utils_canonicalize_filename:
 * @filename : an absolute local path.
 *
 * Translates the @filename to a canonicalized form.
 **/
void
exo_mount_utils_canonicalize_filename (gchar *filename)
{
  gboolean last_was_slash = FALSE;
  gchar   *p;
  gchar   *q;

  for (p = q = filename; *p != '\0'; )
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
          result = exo_mount_utils_is_same_device (mntent->mnt_fsname, device_file);

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
          result = exo_mount_utils_is_same_device (mntent.mnt_special, device_file);

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
          result = exo_mount_utils_is_same_device (mntbuf[n].f_mntfromname, device_file);

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



/**
 * exo_mount_utils_is_same_device:
 * @device_file1 : absolute path to the first device file.
 * @device_file2 : absolute path to the second device file.
 *
 * Returns %TRUE if @device_file1 and @device_file2 refer to
 * the same device (comparing their device major and minor
 * numbers).
 *
 * Return value: %TRUE if @device_file1 and @device_file2
 *               refer to the same device, %FALSE otherwise.
 **/
gboolean
exo_mount_utils_is_same_device (const gchar *device_file1,
                                const gchar *device_file2)
{
  struct stat statb1;
  struct stat statb2;

  g_return_val_if_fail (device_file1 != NULL, FALSE);
  g_return_val_if_fail (device_file2 != NULL, FALSE);

  /* both device file names must be absolute paths */
  if (!g_path_is_absolute (device_file1) || !g_path_is_absolute (device_file2))
    return FALSE;

  /* try to stat both device files */
  if (stat (device_file1, &statb1) < 0 || stat (device_file2, &statb2) < 0)
    return FALSE;

  /* must be both a character or a block device whose rdev matches */
  return ((S_ISBLK (statb1.st_mode) && S_ISBLK (statb2.st_mode))
       || (S_ISCHR (statb1.st_mode) && S_ISCHR (statb1.st_mode)))
      && (statb1.st_rdev == statb2.st_rdev);
}


