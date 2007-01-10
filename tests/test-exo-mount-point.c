/* $Id$ */
/*-
 * Copyright (c) 2007 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <exo/exo.h>



static void
print_mount_point (const ExoMountPoint *mount_point)
{
  g_print (" - %s\t%s\t%s", mount_point->device, mount_point->folder, mount_point->fstype);
  if ((mount_point->flags & EXO_MOUNT_POINT_READ_ONLY) != 0)
    g_print (" (ro)");
  g_print ("\n");
}



int
main (int argc, char **argv)
{
  GError *err = NULL;
  GSList *mount_points;
  GSList *lp;

  mount_points = exo_mount_point_list_active (&err);
  if (G_UNLIKELY (err != NULL))
    {
      g_printerr ("Failed to query active mount points: %s\n", err->message);
      g_error_free (err);
      return EXIT_FAILURE;
    }
  g_print ("Active mount points:\n--------------------\n");
  for (lp = mount_points; lp != NULL; lp = lp->next)
    {
      print_mount_point (lp->data);
      exo_mount_point_free (lp->data);
    }
  g_slist_free (mount_points);
  g_print ("\n");

  mount_points = exo_mount_point_list_configured (&err);
  if (G_UNLIKELY (err != NULL))
    {
      g_printerr ("Failed to query configured mount points: %s\n", err->message);
      g_error_free (err);
      return EXIT_FAILURE;
    }
  g_print ("Configured mount points:\n------------------------\n");
  for (lp = mount_points; lp != NULL; lp = lp->next)
    {
      print_mount_point (lp->data);
      exo_mount_point_free (lp->data);
    }
  g_slist_free (mount_points);
  g_print ("\n");

  return EXIT_SUCCESS;
}


