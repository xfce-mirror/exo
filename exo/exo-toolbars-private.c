/*-
 * Copyright (c) 2004 os-cillation e.K.
 * Copyright (c) 2003 Marco Pesenti Gritti
 *
 * Written by Benedikt Meurer <benny@xfce.org>.
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

#include <exo/exo-toolbars-private.h>
#include <exo/exo-alias.h>



static void
fake_expose_widget (GtkWidget *widget,
                    GdkPixmap *pixmap)
{
  GdkWindow *tmp_window;
  GdkEventExpose event;

  event.type = GDK_EXPOSE;
  event.window = pixmap;
  event.send_event = FALSE;
  gtk_widget_get_allocation (widget, &event.area);
  event.region = NULL;
  event.count = 0;

  tmp_window = gtk_widget_get_window (widget);
  gtk_widget_set_window (widget, pixmap);
  gtk_widget_send_expose (widget, (GdkEvent *) &event);
  gtk_widget_set_window (widget, tmp_window);
}



static GdkPixbuf*
new_pixbuf_from_widget (GtkWidget *widget)
{
  GtkWidget *window;
  GdkPixbuf *pixbuf;
  GtkRequisition requisition;
  GtkAllocation allocation;
  GdkPixmap *pixmap;
  gint icon_width;
  gint icon_height;

#define DEFAULT_ICON_HEIGHT 20
#define DEFAULT_ICON_WIDTH 0

  icon_width = DEFAULT_ICON_WIDTH;

  if (!gtk_icon_size_lookup_for_settings (gtk_settings_get_default (),
                                          GTK_ICON_SIZE_LARGE_TOOLBAR,
                                          NULL, &icon_height))
    {
      icon_height = DEFAULT_ICON_HEIGHT;
    }

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_container_add (GTK_CONTAINER (window), widget);
  gtk_widget_realize (window);
  gtk_widget_show (widget);
  gtk_widget_realize (widget);
  gtk_widget_map (widget);

  /* Gtk will never set the width or height of a window to 0. So setting the width to
   * 0 and than getting it will provide us with the minimum width needed to render
   * the icon correctly, without any additional window background noise.
   * This is needed mostly for pixmap based themes.
   */
  gtk_window_set_default_size (GTK_WINDOW (window), icon_width, icon_height);
  gtk_window_get_size (GTK_WINDOW (window),&icon_width, &icon_height);

  gtk_widget_size_request (window, &requisition);
  allocation.x = 0;
  allocation.y = 0;
  allocation.width = icon_width;
  allocation.height = icon_height;
  gtk_widget_size_allocate (window, &allocation);
  gtk_widget_size_request (window, &requisition);

  /* Create a pixmap */
  pixmap = gdk_pixmap_new (gtk_widget_get_window (window), icon_width, icon_height, -1);
  gdk_drawable_set_colormap (GDK_DRAWABLE (pixmap), gtk_widget_get_colormap (window));

  /* Draw the window */
  gtk_widget_ensure_style (window);
  g_assert (gtk_widget_get_style (window));
  g_assert (gtk_widget_get_style (window)->font_desc);

  fake_expose_widget (window, pixmap);
  fake_expose_widget (widget, pixmap);

  pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, icon_width, icon_height);
  gdk_pixbuf_get_from_drawable (pixbuf, pixmap, NULL, 0, 0, 0, 0, icon_width, icon_height);

  return pixbuf;
}



static void
update_separator_image (GtkImage *image)
{
  GdkPixbuf *pixbuf;

  pixbuf = _exo_toolbars_new_separator_pixbuf ();
  gtk_image_set_from_pixbuf (image, pixbuf);
  g_object_unref (G_OBJECT (pixbuf));
}



static gboolean
style_set (GtkWidget *image)
{
  update_separator_image (GTK_IMAGE (image));
  return FALSE;
}



/**
 * _exo_toolbars_new_separator_pixbuf:
 *
 * Creates a new #GdkPixbuf with the picture
 * of a vertical separator.
 *
 * Returns: A #GdkPixbuf.
 **/
GdkPixbuf*
_exo_toolbars_new_separator_pixbuf (void)
{
  GtkWidget *separator;
  GdkPixbuf *pixbuf;

  separator = gtk_vseparator_new ();
  pixbuf = new_pixbuf_from_widget (separator);
  gtk_widget_destroy (separator);

  return pixbuf;
}



/**
 * _exo_toolbars_new_separator_image:
 *
 * Creates a #GtkImage with the picture of a
 * vertical separator in it.
 *
 * Returns: A #GtkImage.
 **/
GtkWidget*
_exo_toolbars_new_separator_image (void)
{
  GtkWidget *image;

  image = gtk_image_new ();
  update_separator_image (GTK_IMAGE (image));
  g_signal_connect (G_OBJECT (image), "style-set",
                    G_CALLBACK (style_set), NULL);

  return image;
}



/**
 * _exo_toolbars_find_action:
 * @ui_manager  : A #GtkUIManager.
 * @name        : The name of an action in @ui_manager.
 *
 * Searches for the last #GtkAction named @name in @ui_manager, returning
 * %NULL on failure.
 *
 * Returns: The last #GtkAction named @name in @ui_manager or %NULL.
 **/
GtkAction*
_exo_toolbars_find_action (GtkUIManager *ui_manager,
                           const gchar  *name)
{
  GtkAction *action = NULL;
  GtkAction *tmp;
  GList     *lp;

  g_return_val_if_fail (GTK_IS_UI_MANAGER (ui_manager), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  for (lp = gtk_ui_manager_get_action_groups (ui_manager); lp != NULL; lp = lp->next)
    {
      tmp = gtk_action_group_get_action (GTK_ACTION_GROUP (lp->data), name);
      if (G_UNLIKELY (tmp != NULL))
        action = tmp;
    }

  return action;
}



/**
 * _exo_toolbars_set_drag_cursor:
 * @widget  : A #GtkWidget.
 *
 * Changes the #GdkCursor to the dragging appearance.
 **/
void
_exo_toolbars_set_drag_cursor (GtkWidget *widget)
{
  GdkCursor *cursor;

  if (G_LIKELY (gtk_widget_get_window (widget) != NULL))
    {
      cursor = gdk_cursor_new_from_name (gtk_widget_get_display (widget), "grabbing");
      gdk_window_set_cursor (gtk_widget_get_window (widget), cursor);
      gdk_cursor_unref (cursor);
    }
}



/**
 * _exo_toolbars_unset_drag_cursor:
 * @widget  : A #GtkWidget.
 *
 * Restores the #GdkCursor to it's default appearance.
 **/
void
_exo_toolbars_unset_drag_cursor (GtkWidget *widget)
{
  if (G_LIKELY (gtk_widget_get_window (widget) != NULL))
    gdk_window_set_cursor (gtk_widget_get_window (widget), NULL);
}
