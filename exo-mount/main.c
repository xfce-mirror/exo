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

#include <glib/gstdio.h>

#include <exo-hal/exo-hal.h>

#include <exo-mount/exo-mount-fstab.h>
#include <exo-mount/exo-mount-hal.h>
#include <exo-mount/exo-mount-utils.h>



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
  { "version", 'v', 0, G_OPTION_ARG_NONE, &opt_version, N_ ("Print version information and exit"), NULL, },
  { NULL, },
};



int
main (int argc, char **argv)
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

  /* canonicalize the device file path */
  exo_mount_utils_canonicalize_filename (opt_device);

  /* check if the device is currently mounted */
  mounted = exo_mount_utils_is_mounted (opt_device, &mounted_readonly);
  if ((!opt_eject && !opt_unmount && mounted)
      || (opt_unmount && !mounted))
    {
      /* pretend that we succeed */
      return EXIT_SUCCESS;
    }

  /* check if a name was found */
  if (G_UNLIKELY (name == NULL || *name == '\0'))
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
      if (icon != NULL && *icon != '\0')
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
  if (!exo_mount_fstab_contains (opt_device))
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
        exo_mount_fstab_unmount (opt_device, &err);
      else if (G_LIKELY (opt_eject))
        exo_mount_fstab_eject (opt_device, &err);
      else
        exo_mount_fstab_mount (opt_device, &err);
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
      && !exo_mount_utils_is_mounted (opt_device, NULL))
    {
      /* although somebody claims that we were successfully, that's not the case */
      g_set_error (&err, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   "Mount operation claims to be successfull, "
                   "but kernel doesn't list the volume as mounted");
    }

  /* check if we failed */
  if (G_UNLIKELY (err != NULL))
    {
err0: /* check if we should display an error dialog */
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
