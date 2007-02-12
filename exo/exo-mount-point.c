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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_STATVFS_H
#include <sys/statvfs.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_UCRED_H
#include <sys/ucred.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
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
#include <stdio.h>  /* Solaris 2.8 needs this before mntent.h */
#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo-mount-point.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-alias.h>



/* define _PATH_FSTAB if undefined */
#ifndef _PATH_FSTAB
#ifdef sun
#define _PATH_FSTAB "/etc/vfstab"
#else
#define _PATH_FSTAB "/etc/fstab"
#endif
#endif



static inline void    exo_mount_point_add_if_matches        (ExoMountPointMatchMask mask,
                                                             const gchar           *mask_device,
                                                             const gchar           *mask_folder,
                                                             const gchar           *mask_fstype,
                                                             const gchar           *real_device,
                                                             const gchar           *real_folder,
                                                             const gchar           *real_fstype,
                                                             gboolean               real_read_only,
                                                             GSList               **mount_points);
static inline GSList *exo_mount_point_list_match_active     (ExoMountPointMatchMask mask,
                                                             const gchar           *device,
                                                             const gchar           *folder,
                                                             const gchar           *fstype,
                                                             GError               **error) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
static inline GSList *exo_mount_point_list_match_configured (ExoMountPointMatchMask mask,
                                                             const gchar           *device,
                                                             const gchar           *folder,
                                                             const gchar           *fstype,
                                                             GError               **error) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;



/* locking required, because most of the routines used aren't thread-safe */
G_LOCK_DEFINE_STATIC (exo_mount_point_list_matched);



static inline void
exo_mount_point_add_if_matches (ExoMountPointMatchMask mask,
                                const gchar           *mask_device,
                                const gchar           *mask_folder,
                                const gchar           *mask_fstype,
                                const gchar           *real_device,
                                const gchar           *real_folder,
                                const gchar           *real_fstype,
                                gboolean               real_read_only,
                                GSList               **mount_points)
{
  ExoMountPoint *mount_point;
  struct stat    statb1;
  struct stat    statb2;

  /* check if device should be matched */
  if ((mask & EXO_MOUNT_POINT_MATCH_DEVICE) != 0)
    {
      /* check if both specify an absolute path */
      if (*mask_device == '/' && *real_device == '/')
        {
          /* check if both refer to the same device, which means both must be
           * a character device or a block device and the rdev's must match.
           */
          if (stat (mask_device, &statb1) < 0 || stat (real_device, &statb2) < 0)
            {
              /* comapre by path/name instead */
              goto match_device_by_path;
            }
          else if (((!S_ISBLK (statb1.st_mode) || !S_ISBLK (statb2.st_mode))
                 && (!S_ISCHR (statb1.st_mode) || !S_ISCHR (statb1.st_mode)))
              || (statb1.st_rdev != statb2.st_rdev))
            {
              /* different devices */
              return;
            }
        }
      else
        {
match_device_by_path:
          /* compare by their paths/names instead */
          if (strcmp (mask_device, real_device) != 0)
            return;
        }
    }

  /* check if folder should be matched */
  if ((mask & EXO_MOUNT_POINT_MATCH_FOLDER) != 0)
    {
      /* just compare the folders by their paths */
      if (strcmp (mask_folder, real_folder) != 0)
        return;
    }

  /* check if fstype should be matched */
  if ((mask & EXO_MOUNT_POINT_MATCH_FSTYPE) != 0)
    {
      /* just compare the file system types by their names */
      if (strcmp (mask_fstype, real_fstype) != 0)
        return;
    }

  /* allocate an ExoMountPoint, we matched */
  mount_point = _exo_slice_new (ExoMountPoint);
  mount_point->flags = real_read_only ? EXO_MOUNT_POINT_READ_ONLY : 0;
  mount_point->device = g_strdup (real_device);
  mount_point->folder = g_strdup (real_folder);
  mount_point->fstype = g_strdup (real_fstype);

  /* and add the mount point to the list */
  *mount_points = g_slist_prepend (*mount_points, mount_point);
}



static inline GSList*
exo_mount_point_list_match_active (ExoMountPointMatchMask mask,
                                   const gchar           *device,
                                   const gchar           *folder,
                                   const gchar           *fstype,
                                   GError               **error)
{
  GSList *mount_points = NULL;

#if defined(HAVE_SETMNTENT) /* Linux */
  struct mntent *mntent;
  FILE          *fp;

  /* try to open the /proc/mounts file */
  fp = setmntent ("/proc/mounts", "r");
  if (G_UNLIKELY (fp == NULL))
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Failed to open file \"%s\": %s"), "/proc/mounts",
                   g_strerror (errno));
      return NULL;
    }

  /* match all included entries */
  for (;;)
    {
      /* grab the next entry */
      mntent = getmntent (fp);
      if (mntent == NULL)
        break;

      /* check if we have a match here */
      exo_mount_point_add_if_matches (mask, device, folder, fstype, mntent->mnt_fsname, mntent->mnt_dir,
                                      mntent->mnt_type, (hasmntopt (mntent, "ro") != NULL), &mount_points);
    }

  /* close the file handle */
  endmntent (fp);
#elif defined(HAVE_GETMNTENT)
  struct mnttab mntent;
  FILE         *fp;

  /* try to open the /etc/mnttab file */
  fp = fopen ("/etc/mnttab", "r");
  if (G_UNLIKELY (fp == NULL))
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Failed to open file \"%s\": %s"), "/etc/mnttab",
                   g_strerror (errno));
      return NULL;
    }

  /* match all included entries */
  for (;;)
    {
      /* grab the next entry */
      if (getmntent (fp, &mntent) != 0)
        break;

      /* check if we have a match here */
      exo_mount_point_add_if_matches (mask, device, folder, fstype, mntent.mnt_special, mntent.mnt_mountp,
                                      mntent.mnt_fstype, (hasmntopt (&mntent, "ro") != NULL), &mount_points);
    }

  /* close the file handle */
  fclose (fp);
#elif defined(HAVE_GETFSSTAT) /* FreeBSD, OpenBSD, DragonFly, older NetBSD */
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
      mntbuf = (struct statfs *) malloc (bufsize);

      /* determine the mount point for the device file */
      mntsize = getfsstat (mntbuf, bufsize, MNT_NOWAIT);
      for (n = 0; n < mntsize; ++n)
        {
          /* check if we have a match here */
          exo_mount_point_add_if_matches (mask, device, folder, fstype, mntbuf[n].f_mntfromname, mntbuf[n].f_mntonname,
                                          mntbuf[n].f_fstypename, ((mntbuf[n].f_flags & MNT_RDONLY) != 0), &mount_points);
        }

      /* release the buffer */
      free (mntbuf);
    }
#elif defined(HAVE_GETVFSSTAT) /* Newer NetBSD */
  struct statvfs *mntbuf = NULL;
  glong           bufsize = 0;
  gint            mntsize;
  gint            n;

  /* determine the number of active mount points */
  mntsize = getvfsstat (NULL, 0, MNT_NOWAIT);
  if (G_LIKELY (mntsize > 0))
    {
      /* allocate a new buffer */
      bufsize = (mntsize + 4) * sizeof (*mntbuf);
      mntbuf = (struct statvfs *) malloc (bufsize);

      /* determine the mount point for the device file */
      mntsize = getvfsstat (mntbuf, bufsize, ST_NOWAIT);
      for (n = 0; n < mntsize; ++n)
        {
          /* check if we have a match here */
          exo_mount_point_add_if_matches (mask, device, folder, fstype,
                                          mntbuf[n].f_mntfromname,
                                          mntbuf[n].f_mntonname,
                                          mntbuf[n].f_fstypename,
                                          ((mntbuf[n].f_flag & MNT_RDONLY) != 0),
                                          &mount_points);
        }

      /* release the buffer */
      free (mntbuf);
    }
#else
#error "Add support for your operating system here."
#endif

  /* return the collected mount points */
  return mount_points;
}



static inline GSList*
exo_mount_point_list_match_configured (ExoMountPointMatchMask mask,
                                       const gchar           *device,
                                       const gchar           *folder,
                                       const gchar           *fstype,
                                       GError               **error)
{
  GSList *mount_points = NULL;

#if defined(HAVE_SETMNTENT) /* Linux */
  struct mntent *mntent;
  FILE          *fp;

  /* try to open the fstab file */
  fp = setmntent (_PATH_FSTAB, "r");
  if (G_UNLIKELY (fp == NULL))
    goto err;

  /* match all included entries */
  for (;;)
    {
      /* grab the next entry */
      mntent = getmntent (fp);
      if (mntent == NULL)
        break;

      /* skip swap entries */
      if (strcmp (mntent->mnt_type, "swap") == 0)
        continue;

      /* check if we have a match here */
      exo_mount_point_add_if_matches (mask, device, folder, fstype, mntent->mnt_fsname, mntent->mnt_dir,
                                      mntent->mnt_type, (hasmntopt (mntent, "ro") != NULL), &mount_points);
    }

  /* close the file handle */
  endmntent (fp);
#elif defined(HAVE_GETMNTENT) /* Solaris */
  struct mnttab mntent;
  FILE         *fp;

  /* try to open the fstab file */
  fp = fopen (_PATH_FSTAB, "r");
  if (G_UNLIKELY (fp == NULL))
    goto err;

  /* match all included entries */
  for (;;)
    {
      /* grab the next entry */
      if (getmntent (fp, &mntent) != 0)
        break;

      /* skip swap entries */
      if (strcmp (mntent.mnt_fstype, "swap") == 0)
        continue;

      /* check if we have a match here */
      exo_mount_point_add_if_matches (mask, device, folder, fstype, mntent.mnt_special, mntent.mnt_mountp,
                                      mntent.mnt_fstype, (hasmntopt (&mntent, "ro") != NULL), &mount_points);
    }

  /* close the file handle */
  fclose (fp);
#elif defined(HAVE_SETFSENT) /* BSD */
  struct fstab *fs;

  /* try to open the fstab file */
  if (setfsent () == 0)
    goto err;

  /* match all included entries */
  for (;;)
    {
      /* grab the next entry */
      fs = getfsent ();
      if (fs == NULL)
        break;

      /* skip special entries */
      if (strcmp (fs->fs_type, FSTAB_SW) == 0
#ifdef FSTAB_DP
          || strcmp (fs->fs_type, FSTAB_DP) == 0
#endif
          || strcmp (fs->fs_type, FSTAB_XX) == 0)
        continue;

      /* check if we have a match here */
      exo_mount_point_add_if_matches (mask, device, folder, fstype, fs->fs_spec, fs->fs_file, fs->fs_vfstype,
                                      (strcmp (fs->fs_type, FSTAB_RO) == 0), &mount_points);
    }

  /* close the file handle */
  endfsent ();
#else
#error "Add support for your operating system here."
#endif

  /* return the collected mount points */
  return mount_points;

err:
  g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
               _("Failed to open file \"%s\": %s"), _PATH_FSTAB,
               g_strerror (errno));
  return NULL;
}



GType
exo_mount_point_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = g_boxed_type_register_static (I_("ExoMountPoint"),
                                           (GBoxedCopyFunc) exo_mount_point_dup,
                                           (GBoxedFreeFunc) exo_mount_point_free);
    }

  return type;
}



/**
 * exo_mount_point_dup:
 * @mount_point : an #ExoMountPoint.
 *
 * Duplicates the specified @mount_point and returns
 * the duplicate. If @mount_point is %NULL, this simply
 * returns %NULL.
 *
 * The caller is responsible to free the returned mount
 * point using exo_mount_point_free() when no longer
 * needed.
 *
 * Return value: a copy of the specified @mount_point.
 *
 * Since: 0.3.1.13
 **/
ExoMountPoint*
exo_mount_point_dup (const ExoMountPoint *mount_point)
{
  ExoMountPoint *duplicate;

  if (G_LIKELY (mount_point != NULL))
    {
      duplicate = _exo_slice_new (ExoMountPoint);
      duplicate->flags = mount_point->flags;
      duplicate->device = g_strdup (mount_point->device);
      duplicate->folder = g_strdup (mount_point->folder);
      duplicate->fstype = g_strdup (mount_point->fstype);
      return duplicate;
    }
  else
    {
      /* duplicating NULL yields NULL */
      return NULL;
    }
}



/**
 * exo_mount_point_free:
 * @mount_point : an #ExoMountPoint.
 *
 * Frees the resources allocated to the specified @mount_point.
 * If @mount_point is %NULL, this function does nothing.
 *
 * Since: 0.3.1.13
 **/
void
exo_mount_point_free (ExoMountPoint *mount_point)
{
  if (G_LIKELY (mount_point != NULL))
    {
      g_free (mount_point->device);
      g_free (mount_point->folder);
      g_free (mount_point->fstype);
      _exo_slice_free (ExoMountPoint, mount_point);
    }
}



/**
 * exo_mount_point_list_matched:
 * @mask   : the mask of flags that have to match for a mount point to be returned.
 * @device : the device file to match if %EXO_MOUNT_POINT_MATCH_DEVICE is specified.
 * @folder : the folder to match if %EXO_MOUNT_POINT_MATCH_FOLDER is specified.
 * @fstype : the file system type to match if %EXO_MOUNT_POINT_MATCH_FSTYPE is specified.
 * @error  : return location for errors or %NULL.
 *
 * Lists mount points matching the given @mask and optionally the parameters @device,
 * @folder and @fstype. If an error occurrs and @error is non-%NULL, the @error will
 * be set to point to a #GError describing the problem, and %NULL will be returned.
 * Note, however, that %NULL may also be returned if no mount points match.
 *
 * If @mask includes %EXO_MOUNT_POINT_MATCH_ACTIVE, the currently active mount points will
 * be matched, that is, the currently mounted file systems, queried from the kernel. Otherwise
 * if %EXO_MOUNT_POINT_MATCH_CONFIGURED is specified, the configured mount points from the
 * file system table (usually <filename>/etc/fstab</filename> or <filename>/etc/vfstab</filename>)
 * will be matched.
 *
 * The caller is responsible to free the returned list using
 * <informalexample><programlisting>
 * g_slist_foreach (list, (GFunc) exo_mount_point_free, NULL);
 * g_slist_free (list);
 * </programlisting></informalexample>
 * when no longer needed.
 *
 * Return value: the list of matching #ExoMountPoint<!---->s.
 *
 * Since: 0.3.1.13
 **/
GSList*
exo_mount_point_list_matched (ExoMountPointMatchMask mask,
                              const gchar           *device,
                              const gchar           *folder,
                              const gchar           *fstype,
                              GError               **error)
{
  GSList *mount_points;

  g_return_val_if_fail ((mask & EXO_MOUNT_POINT_MATCH_DEVICE) == 0 || device != NULL, NULL);
  g_return_val_if_fail ((mask & EXO_MOUNT_POINT_MATCH_FOLDER) == 0 || folder != NULL, NULL);
  g_return_val_if_fail ((mask & EXO_MOUNT_POINT_MATCH_FSTYPE) == 0 || fstype != NULL, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

#if defined(HAVE_GETFSSTAT) && !defined(HAVE_GETMNTENT) && !defined(HAVE_SETMNTENT) 
  /* getfsstat(2) is really thread-safe, so we can skip locking there */
  if ((mask & EXO_MOUNT_POINT_MATCH_CONFIGURED) == EXO_MOUNT_POINT_MATCH_CONFIGURED)
#endif
    {
      /* acquire the mount point listing lock */
      G_LOCK (exo_mount_point_list_matched);
    }

  /* list the requested mount points */
  mount_points = ((mask & EXO_MOUNT_POINT_MATCH_CONFIGURED) == 0)
               ? exo_mount_point_list_match_active (mask, device, folder, fstype, error)
               : exo_mount_point_list_match_configured (mask, device, folder, fstype, error);

#if defined(HAVE_GETFSSTAT) && !defined(HAVE_GETMNTENT) && !defined(HAVE_SETMNTENT) 
  /* getfsstat(2) is really thread-safe, so we can skip locking there */
  if ((mask & EXO_MOUNT_POINT_MATCH_CONFIGURED) == EXO_MOUNT_POINT_MATCH_CONFIGURED)
#endif
    {
      /* release the mount point listing lock */
      G_UNLOCK (exo_mount_point_list_matched);
    }

  return mount_points;
}



#define __EXO_MOUNT_POINT_C__
#include <exo/exo-aliasdef.c>
