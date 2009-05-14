/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2009 Jannis Pohlmann <jannis@xfce.org>
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free 
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <glib.h>

#include <libxfce4util/libxfce4util.h>

#include <exo-thumbnailers/exo-thumbnailer-service.h>



int
main (int    argc,
      char **argv)
{
  ExoThumbnailerService *service;
  GMainLoop             *main_loop;
  GError                *error = NULL;

  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  g_set_application_name (_("Exo Thumbnailer Service"));

#ifdef G_ENABLE_DEBUG
  g_log_set_always_fatal (G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);
#endif

  /* initialize the type system */
  g_type_init ();

  /* initialize the threading system */
  if (!g_thread_supported ())
    g_thread_init (NULL);

  service = exo_thumbnailer_service_new_unique (&error);
  if (G_UNLIKELY (service == NULL))
    {
      g_critical (_("Failed to start the exo thumbnailer service: %s"), error->message);
      g_error_free (error);
      return EXIT_FAILURE;
    }

  main_loop = g_main_loop_new (NULL, FALSE);

  g_main_loop_run (main_loop);

  g_object_unref (service);
  g_main_loop_unref (main_loop);

  return EXIT_SUCCESS;
}
