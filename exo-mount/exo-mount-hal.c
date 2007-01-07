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

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libhal-storage.h>

#include <exo-hal/exo-hal.h>

#include <exo-mount/exo-mount-hal.h>



static gboolean exo_mount_hal_init            (GError   **error);
static void     exo_mount_hal_propagate_error (GError   **error,
                                               DBusError *derror);



struct _ExoMountHalDevice
{
  gchar            *udi;
  LibHalDrive      *drive;
  LibHalVolume     *volume;

  /* device internals */
  gchar            *file;
  gchar            *name;

  /* file system options */
  gchar           **fsoptions;
  const gchar      *fstype;
  LibHalVolumeUsage fsusage;
};



static LibHalContext  *hal_context = NULL;
static DBusConnection *dbus_connection = NULL;



static gboolean
exo_mount_hal_init (GError **error)
{
  DBusError derror;

  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* check if HAL support is already initialized */
  if (G_LIKELY (hal_context == NULL))
    {
      /* initialize D-Bus error */
      dbus_error_init (&derror);

      /* try to connect to the system bus */
      dbus_connection = dbus_bus_get (DBUS_BUS_SYSTEM, &derror);
      if (G_LIKELY (dbus_connection != NULL))
        {
          /* try to allocate a new HAL context */
          hal_context = libhal_ctx_new ();
          if (G_LIKELY (hal_context != NULL))
            {
              /* setup the D-Bus connection for the HAL context */
              libhal_ctx_set_dbus_connection (hal_context, dbus_connection);

              /* try to initialize the HAL context */
              libhal_ctx_init (hal_context, &derror);
            }
          else
            {
              /* record the allocation failure of the context */
              dbus_set_error_const (&derror, DBUS_ERROR_NO_MEMORY, g_strerror (ENOMEM));
            }
        }

      /* check if we failed */
      if (dbus_error_is_set (&derror))
        {
          /* check if a HAL context was allocated */
          if (G_UNLIKELY (hal_context != NULL))
            {
              /* drop the allocated HAL context */
              libhal_ctx_shutdown (hal_context, NULL);
              libhal_ctx_free (hal_context);
              hal_context = NULL;
            }

          /* propagate the error */
          exo_mount_hal_propagate_error (error, &derror);
        }
    }

  return (hal_context != NULL);
}



static void
exo_mount_hal_propagate_error (GError   **error,
                               DBusError *derror)
{
  g_return_if_fail (error == NULL || *error == NULL);

  /* check if we need to propragate an error */
  if (G_LIKELY (derror != NULL && dbus_error_is_set (derror)))
    {
      /* propagate the error */
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, "%s", derror->message);

      /* reset the D-Bus error */
      dbus_error_free (derror);
    }
}



/**
 * exo_mount_hal_device_from_udi:
 * @udi   : UDI of a volume or drive.
 * @error : return location for errors or %NULL.
 *
 * The returned object must be freed when no longer
 * needed using exo_mount_hal_device_free().
 *
 * Return value: the #ExoMountHalDevice for the @udi
 *               or %NULL in case of an error.
 **/
ExoMountHalDevice*
exo_mount_hal_device_from_udi (const gchar *udi,
                               GError     **error)
{
  ExoMountHalDevice *device = NULL;
  DBusError          derror;
  gchar            **interfaces;
  gint               n;

  g_return_val_if_fail (udi != NULL, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  /* make sure the HAL support is initialized */
  if (!exo_mount_hal_init (error))
    return NULL;

  /* initialize D-Bus error */
  dbus_error_init (&derror);

  /* determine the info.interfaces property of the device */
  interfaces = libhal_device_get_property_strlist (hal_context, udi, "info.interfaces", &derror);
  if (G_UNLIKELY (interfaces == NULL))
    {
err0: exo_mount_hal_propagate_error (error, &derror);
      goto out;
    }

  /* verify that we have a mountable device here */
  for (n = 0; interfaces[n] != NULL; ++n)
    if (strcmp (interfaces[n], "org.freedesktop.Hal.Device.Volume") == 0)
      break;
  if (G_UNLIKELY (interfaces[n] == NULL))
    {
      /* definitely not a device that we're able to mount, eject or unmount */
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Given device \"%s\" is not a volume or drive"), udi);
      goto out;
    }

  /* setup the device struct */
  device = g_new0 (ExoMountHalDevice, 1);
  device->udi = g_strdup (udi);

  /* check if we have a volume here */
  device->volume = libhal_volume_from_udi (hal_context, udi);
  if (G_LIKELY (device->volume != NULL))
    {
      /* determine the storage drive for the volume */
      device->drive = libhal_drive_from_udi (hal_context, libhal_volume_get_storage_device_udi (device->volume));
      if (G_LIKELY (device->drive != NULL))
        {
          /* setup the device internals */
          device->file = g_strdup (libhal_volume_get_device_file (device->volume));
          device->name = exo_hal_volume_compute_display_name (hal_context, device->volume, device->drive);

          /* setup the file system internals */
          device->fstype = libhal_volume_get_fstype (device->volume);
          device->fsusage = libhal_volume_get_fsusage (device->volume);
        }
    }
  else
    {
      /* check if we have a drive here (i.e. floppy) */
      device->drive = libhal_drive_from_udi (hal_context, udi);
      if (G_LIKELY (device->drive != NULL))
        {
          /* setup the device internals */
          device->file = g_strdup (libhal_drive_get_device_file (device->drive));
          device->name = exo_hal_drive_compute_display_name (hal_context, device->drive);

          /* setup the file system internals */
          device->fstype = "";
          device->fsusage = LIBHAL_VOLUME_USAGE_MOUNTABLE_FILESYSTEM;
        }
    }

  /* determine the valid mount options from the UDI */
  device->fsoptions = libhal_device_get_property_strlist (hal_context, udi, "volume.mount.valid_options", &derror);
  if (G_UNLIKELY (device->file == NULL || device->name == NULL || device->fsoptions == NULL))
    {
      exo_mount_hal_device_free (device);
      goto err0;
    }

  /* check if we failed */
  if (G_LIKELY (device->drive == NULL))
    {
      /* definitely not a device that we're able to mount, eject or unmount */
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Given device \"%s\" is not a volume or drive"), udi);
      exo_mount_hal_device_free (device);
      device = NULL;
    }

out:
  /* cleanup */
  libhal_free_string_array (interfaces);

  return device;
}



/**
 * exo_mount_hal_device_from_file:
 * @file  : absolute path to a device file.
 * @error : return location for errors or %NULL.
 *
 * The returned object must be freed using
 * exo_mount_hal_device_free() when no longer
 * needed.
 *
 * Return value: the #ExoMountHalDevice for the device
 *               @file, or %NULL in case of an error.
 **/
ExoMountHalDevice*
exo_mount_hal_device_from_file (const gchar *file,
                                GError     **error)
{
  ExoMountHalDevice *device = NULL;
  DBusError          derror;
  gchar            **interfaces;
  gchar            **udis;
  gint               n_udis;
  gint               n, m;

  g_return_val_if_fail (g_path_is_absolute (file), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  /* make sure the HAL support is initialized */
  if (!exo_mount_hal_init (error))
    return NULL;

  /* initialize D-Bus error */
  dbus_error_init (&derror);

  /* query matching UDIs from HAL */
  udis = libhal_manager_find_device_string_match (hal_context, "block.device", file, &n_udis, &derror);
  if (G_UNLIKELY (udis == NULL))
    {
      /* propagate the error */
      exo_mount_hal_propagate_error (error, &derror);
      return NULL;
    }

  /* look for an UDI that specifies the Volume interface */
  for (n = 0; n < n_udis; ++n)
    {
      /* check if we should ignore this device */
      if (libhal_device_get_property_bool (hal_context, udis[n], "info.ignore", NULL))
        continue;

      /* determine the info.interfaces property of the device */
      interfaces = libhal_device_get_property_strlist (hal_context, udis[n], "info.interfaces", NULL);
      if (G_UNLIKELY (interfaces == NULL))
        continue;

      /* check if we have a mountable device here */
      for (m = 0; interfaces[m] != NULL; ++m)
        if (strcmp (interfaces[m], "org.freedesktop.Hal.Device.Volume") == 0)
          break;

      /* check if it's a usable device */
      if (interfaces[m] != NULL)
        {
          libhal_free_string_array (interfaces);
          break;
        }

      /* next one, please */
      libhal_free_string_array (interfaces);
    }

  /* check if we have an UDI */
  if (G_LIKELY (n < n_udis))
    {
      /* try to query the device from the HAL daemon */
      device = exo_mount_hal_device_from_udi (udis[n], error);
    }
  else
    {
      /* tell the caller that no matching device was found */
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Device \"%s\" not found in file system device table"), file);
    }

  /* cleanup */
  libhal_free_string_array (udis);

  return device;
}



/**
 * exo_mount_hal_device_free:
 * @device : an #ExoMountHalDevice or %NULL.
 *
 * Releases the resources allocated to the @device.
 **/
void
exo_mount_hal_device_free (ExoMountHalDevice *device)
{
  /* check if we have a device */
  if (G_LIKELY (device != NULL))
    {
      libhal_free_string_array (device->fsoptions);
      libhal_volume_free (device->volume);
      libhal_drive_free (device->drive);
      g_free (device->file);
      g_free (device->name);
      g_free (device->udi);
      g_free (device);
    }
}



/**
 * exo_mount_hal_device_get_file:
 * @device : an #ExoMountHalDevice.
 *
 * Returns the path of the @device file. The caller
 * is responsible to free the returned string using
 * g_free() when no longer needed.
 *
 * Return value: the @device file path.
 **/
gchar*
exo_mount_hal_device_get_file (ExoMountHalDevice *device)
{
  g_return_val_if_fail (device != NULL, NULL);
  return g_strdup (device->file);
}



/**
 * exo_mount_hal_device_get_name:
 * @device : an #ExoMountHalDevice.
 *
 * Returns the visible name of the @device. The caller
 * is responsible to free the returned string using
 * g_free() when no longer needed.
 *
 * Return value: the @device<!---->s visible name.
 **/
gchar*
exo_mount_hal_device_get_name (ExoMountHalDevice *device)
{
  g_return_val_if_fail (device != NULL, NULL);
  return g_strdup (device->name);
}



/**
 * exo_mount_hal_device_get_icon:
 * @device : an #ExoMountHalDevice.
 *
 * Returns the icon name for the @device or %NULL if no
 * icon could be determined. The caller is responsible
 * to free the returned string icon using g_free() when
 * no longer needed.
 *
 * Return value: the icon name for @device or %NULL.
 **/
gchar*
exo_mount_hal_device_get_icon (ExoMountHalDevice *device)
{
  GtkIconTheme *icon_theme;
  gchar        *icon_name = NULL;
  GList        *icon_list;
  GList        *lp;

  g_return_val_if_fail (device != NULL, NULL);

  /* compute the list of possible icons for the device */
  icon_list = G_UNLIKELY (device->volume == NULL)
            ? exo_hal_drive_compute_icon_list (hal_context, device->drive)
            : exo_hal_volume_compute_icon_list (hal_context, device->volume, device->drive);

  /* determine the default icon theme */
  icon_theme = gtk_icon_theme_get_default ();

  /* look for a usable icon in the list */
  for (lp = icon_list; lp != NULL; lp = lp->next)
    if (gtk_icon_theme_has_icon (icon_theme, lp->data))
      {
        icon_name = g_strdup (lp->data);
        break;
      }

  /* release the icon list */
  g_list_foreach (icon_list, (GFunc) g_free, NULL);
  g_list_free (icon_list);

  /* last fallback is "gnome-dev-removable" */
  if (G_UNLIKELY (icon_name == NULL))
    icon_name = g_strdup ("gnome-dev-removable");

  return icon_name;
}



/**
 * exo_mount_hal_device_is_readonly:
 * @device : an #ExoMountHalDevice.
 *
 * Guesses whether the @device is most probably
 * a read-only data storage (i.e. a CD-ROM).
 *
 * Return value: %TRUE if readonly, %FALSE otherwise.
 **/
gboolean
exo_mount_hal_device_is_readonly (ExoMountHalDevice *device)
{
  g_return_val_if_fail (device != NULL, FALSE);

  /* the "volume.is_mounted_read_only" property might be a good start */
  if (libhal_device_get_property_bool (hal_context, device->udi, "volume.is_mounted_read_only", NULL))
    return TRUE;

  /* otherwise guess based on the drive type */
  switch (libhal_drive_get_type (device->drive))
    {
    /* CD-ROMs and floppies are read-only... */
    case LIBHAL_DRIVE_TYPE_CDROM:
    case LIBHAL_DRIVE_TYPE_FLOPPY:
      return TRUE;

    /* ...everything else is writable */
    default:
      return FALSE;
    }
}



/**
 * exo_mount_hal_device_eject:
 * @device : an #ExoMountHalDevice.
 * @error  : return location for errors or %NULL.
 *
 * Ejects the specified @device, returns %TRUE if
 * successfull, %FALSE if an error occurred.
 *
 * Return value: %TRUE if ejected, %FALSE otherwise.
 **/
gboolean
exo_mount_hal_device_eject (ExoMountHalDevice *device,
                            GError           **error)
{
  const gchar **options = { NULL };
  const guint   n_options = 0;
  DBusMessage  *message;
  DBusMessage  *result;
  DBusError     derror;

  g_return_val_if_fail (device != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* allocate the D-Bus message for the "Eject" method */
  message = dbus_message_new_method_call ("org.freedesktop.Hal", device->udi, "org.freedesktop.Hal.Device.Volume", "Eject");
  if (G_UNLIKELY (message == NULL))
    {
      /* out of memory */
oom:  g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_NOMEM, g_strerror (ENOMEM));
      return FALSE;
    }

  /* append the (empty) eject options array */
  if (!dbus_message_append_args (message, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &options, n_options, DBUS_TYPE_INVALID))
    {
      dbus_message_unref (message);
      goto oom;
    }

  /* initialize D-Bus error */
  dbus_error_init (&derror);

  /* send the message to the HAL daemon and block for the reply */
  result = dbus_connection_send_with_reply_and_block (dbus_connection, message, -1, &derror);
  if (G_LIKELY (result != NULL))
    {
      /* check if an error was returned */
      if (dbus_message_get_type (result) == DBUS_MESSAGE_TYPE_ERROR)
        dbus_set_error_from_message (&derror, result);

      /* release the result */
      dbus_message_unref (result);
    }

  /* release the message */
  dbus_message_unref (message);

  /* check if we failed */
  if (G_UNLIKELY (dbus_error_is_set (&derror)))
    {
      /* try to translate the error appropriately */
      if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.PermissionDenied") == 0)
        {
          /* TRANSLATORS: The user tried to eject a device although he's not privileged to do so. */
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("You are not privileged to eject the volume \"%s\""), device->name);
        }
      else if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.Busy") == 0)
        {
          /* TRANSLATORS: An application is blocking a mounted volume from being ejected. */
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("An application is preventing the volume \"%s\" from being ejected"), device->name);
        }
      else
        {
          /* no precise error message, use the HAL one */
          exo_mount_hal_propagate_error (error, &derror);
        }

      /* release the DBus error */
      dbus_error_free (&derror);
      return FALSE;
    }

  return TRUE;
}



/**
 * exo_mount_hal_device_mount:
 * @device : an #ExoMountHalDevice.
 * @error  : return location for errors or %NULL.
 *
 * Mounts the specified @device and returns %TRUE if
 * successfull, %FALSE in case of an error.
 *
 * Return value: %TRUE if mounted, %FALSE otherwise.
 **/
gboolean
exo_mount_hal_device_mount (ExoMountHalDevice *device,
                            GError           **error)
{
  DBusMessage *message;
  DBusMessage *result;
  DBusError    derror;
  gchar       *mount_point;
  gchar      **options;
  gchar       *fstype;
  gchar       *s;
  gint         m, n = 0;

  g_return_val_if_fail (device != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* determine the required mount options */
  options = g_new0 (gchar *, 20);
  for (m = 0; device->fsoptions[m] != NULL; ++m)
    {
      /* this is currently mostly Linux specific noise */
      if (strcmp (device->fsoptions[m], "uid=") == 0
          && (strcmp (device->fstype, "vfat") == 0
           || strcmp (device->fstype, "iso9660") == 0
           || strcmp (device->fstype, "udf") == 0
           || device->volume == NULL))
        {
          options[n++] = g_strdup_printf ("uid=%u", (guint) getuid ());
        }
      else if (strcmp (device->fsoptions[m], "shortname=") == 0
            && strcmp (device->fstype, "vfat") == 0)
        {
          options[n++] = g_strdup_printf ("shortname=winnt");
        }
      else if (strcmp (device->fsoptions[m], "sync") == 0
            && device->volume == NULL)
        {
          /* non-pollable drive... */
          options[n++] = g_strdup ("sync");
        }
      else if (strcmp (device->fsoptions[m], "longnames") == 0
            && strcmp (device->fstype, "vfat") == 0)
        {
          /* however this one is FreeBSD specific */
          options[n++] = g_strdup ("longnames");
        }
    }

  /* try to determine a usable mount point */
  if (G_LIKELY (device->volume != NULL))
    {
      /* maybe we can use the volume's label... */
      mount_point = (gchar *) libhal_volume_get_label (device->volume);
    }
  else
    {
      /* maybe we can use the the textual type... */
      mount_point = (gchar *) libhal_drive_get_type_textual (device->drive);
    }

  /* make sure that the mount point is usable (i.e. does not contain G_DIR_SEPARATOR's) */
  mount_point = (mount_point != NULL && *mount_point != '\0')
              ? exo_str_replace (mount_point, G_DIR_SEPARATOR_S, "_") 
              : g_strdup ("");

  /* let HAL guess the fstype */
  fstype = g_strdup ("");

  /* setup the D-Bus error */
  dbus_error_init (&derror);

  /* now several times... */
  for (;;)
    {
      /* prepare the D-Bus message for the "Mount" method */
      message = dbus_message_new_method_call ("org.freedesktop.Hal", device->udi, "org.freedesktop.Hal.Device.Volume", "Mount");
      if (G_UNLIKELY (message == NULL))
        {
oom:      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_NOMEM, g_strerror (ENOMEM));
          g_strfreev (options);
          g_free (mount_point);
          g_free (fstype);
          return FALSE;
        }

      /* append the message parameters */
      if (!dbus_message_append_args (message,
                                     DBUS_TYPE_STRING, &mount_point,
                                     DBUS_TYPE_STRING, &fstype,
                                     DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &options, n,
				                             DBUS_TYPE_INVALID))
        {
          dbus_message_unref (message);
          goto oom;
        }

      /* send the message to the HAL daemon */
      result = dbus_connection_send_with_reply_and_block (dbus_connection, message, -1, &derror);
      if (G_LIKELY (result != NULL))
        {
          /* check if an error was returned */
          if (dbus_message_get_type (result) == DBUS_MESSAGE_TYPE_ERROR)
            dbus_set_error_from_message (&derror, result);

          /* release the result */
          dbus_message_unref (result);
        }

      /* release the messages */
      dbus_message_unref (message);

      /* check if we succeed */
      if (!dbus_error_is_set (&derror))
        break;

      /* check if the device was already mounted */
      if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.AlreadyMounted") == 0)
        {
          dbus_error_free (&derror);
          break;
        }

      /* check if the specified mount point was invalid */
      if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.InvalidMountpoint") == 0 && *mount_point != '\0')
        {
          /* try again without a mount point */
          g_free (mount_point);
          mount_point = g_strdup ("");

          /* reset the error */
          dbus_error_free (&derror);
          continue;
        }

      /* check if the specified mount point is not available */
      if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.MountPointNotAvailable") == 0 && *mount_point != '\0')
        {
          /* try again with a new mount point */
          s = g_strconcat (mount_point, "_", NULL);
          g_free (mount_point);
          mount_point = s;

          /* reset the error */
          dbus_error_free (&derror);
          continue;
        }

#if defined(__FreeBSD__)
      /* check if an unknown error occurred while trying to mount a floppy */
      if (strcmp (derror.name, "org.freedesktop.Hal.Device.UnknownError") == 0
          && libhal_drive_get_type (device->drive) == LIBHAL_DRIVE_TYPE_FLOPPY)
        {
          /* check if no file system type was specified */
          if (G_LIKELY (*fstype == '\0'))
            {
              /* try again with msdosfs */
              g_free (fstype);
              fstype = g_strdup ("msdosfs");

              /* reset the error */
              dbus_error_free (&derror);
              continue;
            }
        }
#endif

      /* it's also possible that we need to include "ro" in the options */
      for (n = 0; options[n] != NULL; ++n)
        if (strcmp (options[n], "ro") == 0)
          break;
      if (G_UNLIKELY (options[n] != NULL))
        {
          /* we already included "ro" in the options, no way
           * to mount that device then... we simply give up.
           */
          break;
        }

      /* add "ro" to the options and try again */
      options[n++] = g_strdup ("ro");

      /* reset the error */
      dbus_error_free (&derror);
    }

  /* cleanup */
  g_strfreev (options);
  g_free (mount_point);
  g_free (fstype);

  /* check if we failed */
  if (dbus_error_is_set (&derror))
    {
      /* try to translate the error appropriately */
      if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.PermissionDenied") == 0) 
        {
          /* TRANSLATORS: User tried to mount a volume, but is not privileged to do so. */
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("You are not privileged to mount the volume \"%s\""), device->name);
        }
      else if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.AlreadyMounted") == 0)
        {
          /* Ups, already mounted, we succeed! */
          dbus_error_free (&derror);
          return TRUE;
        }
      else
        {
          /* unknown error, use HAL's message */
          exo_mount_hal_propagate_error (error, &derror);
        }

      /* release D-Bus error */
      dbus_error_free (&derror);
      return FALSE;
    }

  return TRUE;
}




/**
 * exo_mount_hal_device_unmount:
 * @device : an #ExoMountHalDevice.
 * @error  : return location for errors or %NULL.
 *
 * Unmounts the specified @device and returns %TRUE if
 * successfull, %FALSE in case of an error.
 *
 * Return value: %TRUE if unmounted, %FALSE otherwise.
 **/
gboolean
exo_mount_hal_device_unmount (ExoMountHalDevice *device,
                              GError           **error)
{
  const gchar **options = { NULL };
  const guint   n_options = 0;
  DBusMessage  *message;
  DBusMessage  *result;
  DBusError     derror;

  g_return_val_if_fail (device != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* allocate the D-Bus message for the "Unmount" method */
  message = dbus_message_new_method_call ("org.freedesktop.Hal", device->udi, "org.freedesktop.Hal.Device.Volume", "Unmount");
  if (G_UNLIKELY (message == NULL))
    {
      /* out of memory */
oom:  g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_NOMEM, g_strerror (ENOMEM));
      return FALSE;
    }

  /* append the (empty) eject options array */
  if (!dbus_message_append_args (message, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &options, n_options, DBUS_TYPE_INVALID))
    {
      dbus_message_unref (message);
      goto oom;
    }

  /* initialize D-Bus error */
  dbus_error_init (&derror);

  /* send the message to the HAL daemon and block for the reply */
  result = dbus_connection_send_with_reply_and_block (dbus_connection, message, -1, &derror);
  if (G_LIKELY (result != NULL))
    {
      /* check if an error was returned */
      if (dbus_message_get_type (result) == DBUS_MESSAGE_TYPE_ERROR)
        dbus_set_error_from_message (&derror, result);

      /* release the result */
      dbus_message_unref (result);
    }

  /* release the message */
  dbus_message_unref (message);

  /* check if we failed */
  if (G_UNLIKELY (dbus_error_is_set (&derror)))
    {
      /* try to translate the error appropriately */
      if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.PermissionDenied") == 0) 
        {
          /* TRANSLATORS: User tried to unmount a volume, but is not privileged to do so. */
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("You are not privileged to unmount the volume \"%s\""), device->name);
        }
      else if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.Busy") == 0)
        {
          /* TRANSLATORS: An application is blocking a volume from being unmounted. */
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("An application is preventing the volume \"%s\" from being unmounted"), device->name);
        }
      else if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.NotMountedByHal") == 0)
        {
          /* TRANSLATORS: HAL can only unmount volumes that were mounted via HAL. */
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("The volume \"%s\" was probably mounted manually on the command line"), device->name);
        }
      else if (strcmp (derror.name, "org.freedesktop.Hal.Device.Volume.NotMounted") == 0)
        {
          /* Ups, volume not mounted, we succeed! */
          dbus_error_free (&derror);
          return TRUE;
        }
      else
        {
          /* unknown error, use the HAL one */
          exo_mount_hal_propagate_error (error, &derror);
        }

      /* release the DBus error */
      dbus_error_free (&derror);
      return FALSE;
    }

  return TRUE;
}

