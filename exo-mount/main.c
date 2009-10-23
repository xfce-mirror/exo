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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <glib/gstdio.h>

#include <gio/gio.h>
#ifdef HAVE_GIO_UNIX
#include <gio/gunixmounts.h>
#endif

#include <exo-hal/exo-hal.h>
#include <exo/exo.h>
#ifdef HAVE_HAL
#include <exo-mount/exo-mount-hal.h>
#endif


/* --- globals --- */
static gboolean opt_eject = FALSE;
static gboolean opt_unmount = FALSE;
static gchar   *opt_hal_udi = NULL;
static gchar   *opt_device = NULL;
static gboolean opt_noui = FALSE;
static gboolean opt_version = FALSE;



/* --- command line options --- */
static GOptionEntry entries[] =
{
  { "eject", 'e', 0, G_OPTION_ARG_NONE, &opt_eject, N_ ("Eject rather than mount"), NULL, },
  { "unmount", 'u', 0, G_OPTION_ARG_NONE, &opt_unmount, N_ ("Unmount rather than mount"), NULL, },
#ifdef HAVE_HAL
  { "hal-udi", 'h', 0, G_OPTION_ARG_STRING, &opt_hal_udi, N_ ("Mount by HAL device UDI"), NULL, },
#else
  { "hal-udi", 'h', 0, G_OPTION_ARG_STRING, &opt_hal_udi, N_ ("Mount by HAL device UDI (not supported)"), NULL, },
#endif
  { "device", 'd', 0, G_OPTION_ARG_FILENAME, &opt_device, N_ ("Mount by device file"), NULL, },
  { "no-ui", 'n', 0, G_OPTION_ARG_NONE, &opt_noui, N_ ("Don't show any dialogs"), NULL, },
  { "version", 'V', 0, G_OPTION_ARG_NONE, &opt_version, N_ ("Print version information and exit"), NULL, },
  { NULL, },
};



static gboolean
exo_mount_device_is_mounted (const gchar *device_file,
                             gboolean    *readonly_return)
{
#ifdef HAVE_GIO_UNIX
  GList           *lp;
  GUnixMountEntry *mount_entry;
  const gchar     *device_path;

  /* get the mounted devices (from mtab) */
  for (lp = g_unix_mounts_get (NULL); lp != NULL; lp = lp->next)
    {
      mount_entry = lp->data;

      device_path = g_unix_mount_get_device_path (mount_entry);
      if (exo_str_is_equal (device_path, device_file))
        {
          if (G_LIKELY (readonly_return != NULL))
            *readonly_return = g_unix_mount_is_readonly (mount_entry);
          return TRUE;
        }
    }
#endif

  return FALSE;
}



#ifdef HAVE_GIO_UNIX
static gboolean
exo_mount_device_lookup (const gchar      *device_file,
                         GUnixMountPoint **mount_point_return,
                         GError          **error)
{
  GList           *lp;
  GUnixMountPoint *mount_point;
  const gchar     *device_path;

  /* get the known devices (from fstab) */
  for (lp = g_unix_mount_points_get (NULL); lp != NULL; lp = lp->next)
    {
      mount_point = lp->data;

      device_path = g_unix_mount_point_get_device_path (mount_point);
      if (exo_str_is_equal (device_path, device_file))
        {
          if (G_LIKELY (mount_point_return != NULL))
            *mount_point_return = mount_point;
          return TRUE;
        }
    }

  /* TRANSLATORS: a device is missing from the file system table (usually /etc/fstab) */
  if (G_LIKELY (error != NULL))
    g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Device \"%s\" not found in file system device table"), device_file);

  return FALSE;
}
#endif



static gboolean
exo_mount_device_is_mount_point (const gchar *device_file)
{
#ifdef HAVE_GIO_UNIX
  return exo_mount_device_lookup (device_file, NULL, NULL);
#else
  return FALSE;
#endif
}



static void
exo_mount_device_execute (const gchar  *device_file,
                          const gchar  *command,
                          GError      **error)
{
  gboolean         result;
  gchar           *standard_error;
  gchar           *command_line;
  gchar           *quoted;
  gint             status;
  const gchar     *argument;
#ifdef HAVE_GIO_UNIX
  GUnixMountPoint *mount_point;

  if (!exo_mount_device_lookup (device_file, &mount_point, error))
    return;
  argument = g_unix_mount_point_get_device_path (mount_point);
#else
  argument = device_file;
#endif

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
        }

      /* release the stderr output */
      g_free (standard_error);
    }

  /* cleanup */
  g_free (command_line);
}



gint
main (gint argc, gchar **argv)
{
#ifdef HAVE_HAL
  ExoMountHalDevice *device;
#endif
  GtkWidget         *dialog;
  gboolean           mounted_readonly = FALSE;
  gboolean           mounted;
  GError            *err = NULL;
  gchar            **nargv;
  gchar             *message;
  gchar             *icon = NULL;
  gchar             *name = NULL;
  GPid               pid = 0;
  gint               n = 0;

  /* initialize the i18n support */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  /* unset the CTYPE so we get the native charset for iocharset */
  setlocale (LC_CTYPE, "");

  /* initialize GTK+ */
  if (!gtk_init_with_args (&argc, &argv, "Xfce mount", entries, GETTEXT_PACKAGE, &err))
    {
      /* check if we have an error message */
      if (G_LIKELY (err == NULL))
        {
          /* no error message, the GUI initialization failed */
          const gchar *display_name = gdk_get_display_arg_name ();
          g_fprintf (stderr, "exo-mount: %s: %s\n", _("Failed to open display"), (display_name != NULL) ? display_name : " ");
        }
      else
        {
          /* yep, there's an error, so print it */
          g_fprintf (stderr, "exo-mount: %s\n", err->message);
        }
      return EXIT_FAILURE;
    }

  /* check if we should print version */
  if (G_UNLIKELY (opt_version))
    {
      g_print ("%s %s\n\n", g_get_prgname (), PACKAGE_VERSION);
      g_print (_("Copyright (c) %s\n"
                 "        os-cillation e.K. All rights reserved.\n\n"
                 "Written by Benedikt Meurer <benny@xfce.org>.\n\n"),
               "2006-2007");
      g_print (_("%s comes with ABSOLUTELY NO WARRANTY,\n"
                 "You may redistribute copies of %s under the terms of\n"
                 "the GNU Lesser General Public License which can be found in the\n"
                 "%s source package.\n\n"), g_get_prgname (), g_get_prgname (), PACKAGE_TARNAME);
      g_print (_("Please report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
      return EXIT_SUCCESS;
    }

#ifndef HAVE_GIO_UNIX
  g_warning (_("%s is compiled without GIO-Unix features. Therefore it will "
               "probably not work on this system."), g_get_prgname ());
#endif

  /* make sure that either a device UDI or file was specified... */
  if (G_UNLIKELY (opt_hal_udi == NULL && opt_device == NULL))
    {
      /* the caller must specify either a HAL UDI or a device file */
      g_printerr ("%s: %s.\n", g_get_prgname (), _("Must specify HAL device UDI or device file"));
      return EXIT_FAILURE;
    }

  /* ...but not both an UDI and a device file */
  if (G_UNLIKELY (opt_hal_udi != NULL && opt_device != NULL))
    {
      /* the caller must specify EITHER a HAL UDI or a device file */
      g_printerr ("%s: %s.\n", g_get_prgname (), _("Must not specify both a HAL device UDI and a device file simultaneously"));
      return EXIT_FAILURE;
    }

#ifndef HAVE_HAL
  /* cannot mount by UDI if HAL is not available */
  if (G_UNLIKELY (opt_hal_udi != NULL))
    {
      g_printerr ("%s: %s.\n", g_get_prgname (), _("Cannot mount by HAL device UDI, because HAL support was disabled for this build"));
      return EXIT_FAILURE;
    }
#else
  /* make sure the UDI passed in is valid */
  if (opt_hal_udi != NULL && !exo_hal_udi_validate (opt_hal_udi, -1, NULL))
    {
      message = g_strdup_printf (_("The specified UDI \"%s\" is not a valid HAL device UDI"), opt_hal_udi);
      g_printerr ("%s: %s.\n", g_get_prgname (), message);
      g_free (message);
      return EXIT_FAILURE;
    }
#endif

  /* infer operation from program name */
  if (strcmp (g_get_prgname (), "exo-unmount") == 0)
    opt_unmount = TRUE;
  else if (strcmp (g_get_prgname (), "exo-eject") == 0)
    opt_eject = TRUE;

  /* verify that only a single option was specified */
  if (G_UNLIKELY (opt_eject && opt_unmount))
    {
      /* the caller must not specify both eject and unmount */
      g_printerr ("%s: %s.\n", g_get_prgname (), _("Cannot eject and unmount simultaneously"));
      return EXIT_FAILURE;
    }

  /* make sure the device file path is absolute */
  if (opt_device != NULL && !g_path_is_absolute (opt_device))
    {
      /* device is always relative to /dev then */
      opt_device = g_build_filename ("/dev", opt_device, NULL);
    }

#ifdef HAVE_HAL
  /* query the device information from the HAL daemon */
  device = (opt_device != NULL)
         ? exo_mount_hal_device_from_file (opt_device, &err)
         : exo_mount_hal_device_from_udi (opt_hal_udi, &err);
  if (G_UNLIKELY (device == NULL))
    goto err0;

  /* determine the device file from the device struct */
  opt_device = exo_mount_hal_device_get_file (device);

  /* determine name and icon of the device */
  icon = exo_mount_hal_device_get_icon (device);
  name = exo_mount_hal_device_get_name (device);

  /* check if the device is most probably a read-only device */
  mounted_readonly = exo_mount_hal_device_is_readonly (device);
#endif

  /* check if the device is currently mounted */
  mounted = exo_mount_device_is_mounted (opt_device, &mounted_readonly);
  if ((!opt_eject && !opt_unmount && mounted)
      || (opt_unmount && !mounted))
    {
      /* pretend that we succeed */
      return EXIT_SUCCESS;
    }

  /* check if a name was found */
  if (G_UNLIKELY (exo_str_is_empty (name)))
    {
      /* release the previous name */
      g_free (name);

      /* default to the path of the device file */
      name = g_filename_display_name (opt_device);
    }

  /* check if we should display a notification */
  if ((opt_eject && mounted) || opt_unmount)
    {
      /* prepare arguments for the unmount notification */
      nargv = g_new (gchar *, 8);
      nargv[n++] = g_strdup (LIBEXECDIR G_DIR_SEPARATOR_S "exo-mount-notify-" LIBEXO_VERSION_API);
      if (mounted_readonly)
        {
          nargv[n++] = g_strdup ("--readonly");
        }
      if (opt_eject)
        {
          nargv[n++] = g_strdup ("--eject");
        }
      if (!exo_str_is_empty (icon))
        {
          nargv[n++] = g_strdup ("--icon");
          nargv[n++] = g_strdup (icon);
        }
      nargv[n++] = g_strdup ("--name");
      nargv[n++] = g_strdup (name);
      nargv[n] = NULL;

      /* try to spawn the unmount notification */
      if (!gdk_spawn_on_screen (gdk_screen_get_default (), NULL, nargv, NULL, 0, NULL, NULL, &pid, NULL))
        {
          /* failed to spawn, don't worry,
           * just remember this fact...
           */
          pid = 0;
        }

      /* cleanup */
      g_strfreev (nargv);
    }

  /* check if the device appears in the fstab,
   * if so, we cannot use HAL to mount it.
   */
#ifdef HAVE_HAL
  if (!exo_mount_device_is_mount_point (opt_device))
    {
      /* perform the requested operation */
      if (G_LIKELY (opt_unmount))
        exo_mount_hal_device_unmount (device, &err);
      else if (G_LIKELY (opt_eject))
        exo_mount_hal_device_eject (device, &err);
      else
        exo_mount_hal_device_mount (device, &err);
    }
  else
#endif
    {
      /* perform the requested operation */
      if (G_LIKELY (opt_unmount))
        exo_mount_device_execute (opt_device, PATH_UMOUNT, &err);
      else if (G_LIKELY (opt_eject))
        exo_mount_device_execute (opt_device, PATH_EJECT, &err);
      else
        exo_mount_device_execute (opt_device, PATH_MOUNT, &err);
    }

  /* check if we have a notification process to kill */
  if (G_LIKELY (pid != 0))
    {
      /* send SIGUSR1 if we succeed, SIGTERM otherwise */
      kill (pid, (err == NULL) ? SIGUSR1 : SIGTERM);

      /* cleanup the PID */
      g_spawn_close_pid (pid);
    }

  /* if we tried to mount, make sure it's really mounted now */
  if (err == NULL && (!opt_eject && !opt_unmount)
      && !exo_mount_device_is_mounted (opt_device, NULL))
    {
      /* although somebody claims that we were successfully, that's not the case */
      g_set_error (&err, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   "Mount operation claims to be successfull, "
                   "but kernel doesn't list the volume as mounted");
    }

  /* check if we failed */
  if (G_UNLIKELY (err != NULL))
    {
#ifdef HAVE_HAL
err0:
#endif
      /* check if we should display an error dialog */
      if (G_LIKELY (!opt_noui))
        {
          /* make sure we can display a name */
          if (G_UNLIKELY (name == NULL))
            {
              /* either device file or UDI */
              if (G_LIKELY (opt_device != NULL))
                name = g_filename_display_name (opt_device);
              else
                name = g_strdup (opt_hal_udi);
            }

          /* determine the appropriate error message */
          if (G_LIKELY (opt_eject))
            message = g_strdup_printf (_("Failed to eject \"%s\""), name);
          else if (G_LIKELY (opt_unmount))
            message = g_strdup_printf (_("Failed to unmount \"%s\""), name);
          else
            message = g_strdup_printf (_("Failed to mount \"%s\""), name);

          /* popup an error message dialog */
          dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s.", message);
          gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", err->message);
          gtk_dialog_run (GTK_DIALOG (dialog));
          gtk_widget_destroy (dialog);
          g_free (message);
        }
      else
        {
          /* otherwise, it's probably called from thunar-vfs,
           * so print the error message to standard error.
           */
          g_printerr ("%s.\n", err->message);
        }

      /* release the error */
      g_error_free (err);
    }

  /* cleanup */
  g_free (name);
  g_free (icon);

  return (err == NULL) ? EXIT_SUCCESS : EXIT_FAILURE;
}
