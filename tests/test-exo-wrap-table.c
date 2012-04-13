/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <exo/exo.h>



static void
fill_table (GtkWidget *table)
{
  GtkWidget *button;
  gchar     *text;
  gint       n;

  for (n = 0; n < 20; ++n)
    {
      text = g_strdup_printf ("Button %d", n);
      button = gtk_button_new_with_label (text);
      gtk_container_add (GTK_CONTAINER (table), button);
      gtk_widget_show (button);
      g_free (text);
    }
}



gint
main (gint argc, gchar **argv)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *vbox;

  g_log_set_always_fatal (G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 350, 300);
  gtk_window_set_title (GTK_WINDOW (window), "ExoWrapTable test");
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  frame = g_object_new (GTK_TYPE_FRAME, "border-width", 0, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
  gtk_widget_show (frame);

  table = exo_wrap_table_new (TRUE);
  gtk_container_add (GTK_CONTAINER (frame), table);
  gtk_widget_show (table);


  button = gtk_check_button_new_with_label ("Homogeneous");
  exo_mutual_binding_new (G_OBJECT (table), "homogeneous", G_OBJECT (button), "active");
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);


  fill_table (table);

  gtk_widget_show (window);

  gtk_main ();

  return EXIT_SUCCESS;
}



