/* $Id$ */
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo/exo-toolbars-private.h>



static void
fake_expose_widget (GtkWidget *widget,
                    GdkPixmap *pixmap)
{
  GdkWindow *tmp_window;
  GdkEventExpose event;

  event.type = GDK_EXPOSE;
  event.window = pixmap;
  event.send_event = FALSE;
  event.area = widget->allocation;
  event.region = NULL;
  event.count = 0;

  tmp_window = widget->window;
  widget->window = pixmap;
  gtk_widget_send_expose (widget, (GdkEvent *) &event);
  widget->window = tmp_window;
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
					  NULL, 
					  &icon_height))
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
  pixmap = gdk_pixmap_new (GDK_DRAWABLE (window->window), icon_width, icon_height, -1);
  gdk_drawable_set_colormap (GDK_DRAWABLE (pixmap), gtk_widget_get_colormap (window));

  /* Draw the window */
  gtk_widget_ensure_style (window);
  g_assert (window->style);
  g_assert (window->style->font_desc);
  
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
 * Return value : A #GdkPixbuf.
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
 * Return value : A #GtkImage.
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
 * Return value : The last #GtkAction named @name in @ui_manager
 *                or %NULL.
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



#ifdef __SUNPRO_C
#pragma align 4 (drag_cursor_data)
#endif
#ifdef __GNUC__
static const guint8 drag_cursor_data[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 drag_cursor_data[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (2304) */
  "\0\0\11\30"
  /* pixdata_type (0x1010002) */
  "\1\1\0\2"
  /* rowstride (96) */
  "\0\0\0`"
  /* width (24) */
  "\0\0\0\30"
  /* height (24) */
  "\0\0\0\30"
  /* pixel_data: */
  "\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\2\0\0"
  "\0\2\0\0\0\2\0\0\0\3\0\0\0\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\2\0\0\0\2"
  "\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\11\0\0\0\0\0\0"
  "\0\0\0\0\0\1\0\0\0\2\0\0\0\3\0\0\0\1\0\0\0\1\0\0\0\2\0\0\0\2\0\0\0\2"
  "\0\0\0\2\0\0\0\2\0\0\0\1\0\0\0\1\0\0\0\0\0\0\0\1\0\0\0\1\0\0\0\1\0\0"
  "\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\10\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\1\0\0\0\7\0\0\0\4\0\0\0\2\0\0\0\3\0\0\0\3\0\0\0\3\0\0\0\2\0\0"
  "\0\31\0\0\0#\0\0\0\30\0\0\0\2\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1"
  "\0\0\0\1\0\0\0\3\0\0\0\2\0\0\0\5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\1\0\0\0\2\0\0\0\6\0\0\0\11\0\0\0\14\0\0\0\14\0\0\0\12\0\0\0\206\0"
  "\0\0\303\0\0\0\205\0\0\0\11\0\0\0\6\0\0\0\10\0\0\0\6\0\0\0\3\0\0\0\2"
  "\0\0\0\4\0\0\0\12\0\0\0\7\0\0\0\5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\4\0\0\0\6\0\0\0V\0\0\0\245\0\0\0\211\0\0\0\4\0\0\0\207\246\246\246"
  "\331\335\335\335\365\245\245\245\333\0\0\0\250\0\0\0\244\0\0\0\243\0"
  "\0\0""9\0\0\0\2\0\0\0\1\0\0\0\1\0\0\0\2\0\0\0\1\0\0\0\1\0\0\0\0\0\0\0"
  "\0\0\0\0\1\0\0\0\6\0\0\0\12\0\0\0\\uuu\271\270\270\270\353\246\246\246"
  "\332\0\0\0\205\0\0\0\267\271\271\271\352\377\377\377\377\270\270\270"
  "\353$$$\312\270\270\270\353\271\271\271\353WWW\246\0\0\0B\0\0\0\4\0\0"
  "\0\13\0\0\0\7\0\0\0\3\0\0\0\4\0\0\0\1\0\0\0\0\0\0\0\2\0\0\0\11\0\0\0"
  "\12\0\0\0\205\220\220\220\341\377\377\377\377\335\335\335\366\0\0\0\307"
  "\0\0\0\304\270\270\270\353\377\377\377\377\271\271\271\352555\314\377"
  "\377\377\377\377\377\377\377eee\327\0\0\0f\0\0\0)\0\0\0[\0\0\0\10\0\0"
  "\0\4\0\0\0\10\0\0\0\3\0\0\0\2\0\0\0\3\0\0\0\11\0\0\0\6\0\0\0[uuu\271"
  "\321\321\321\362\350\350\350\371ddd\332\22\22\22\311\270\270\270\354"
  "\377\377\377\377\271\271\271\353555\314\377\377\377\377\377\377\377\377"
  "eee\327\0\0\0\210;;;{^^^\300\0\0\0E\0\0\0\15\0\0\0\10\0\0\0\6\0\0\0\5"
  "\0\0\0\5\0\0\0\10\0\0\0\4\0\0\0\12\0\0\0iddd\330\377\377\377\377\377"
  "\377\377\377444\320\267\267\267\354\377\377\377\377\270\270\270\3545"
  "55\316\377\377\377\377\377\377\377\377eee\327\33\33\33\311\201\201\201"
  "\337\335\335\335\365\0\0\0\302\0\0\0\40\0\0\0\4\0\0\0\2\0\0\0\3\0\0\0"
  "\5\0\0\0\3\0\0\0\6\0\0\0\13\0\0\0jddd\331\377\377\377\377\377\377\377"
  "\377444\320\270\270\270\354\377\377\377\377\267\267\267\354444\320\377"
  "\377\377\377\377\377\377\377eee\326\221\221\221\340\377\377\377\377\335"
  "\335\335\365\0\0\0\301\0\0\0\40\0\0\0\4\0\0\0\4\0\0\0\5\0\0\0\7\0\0\0"
  "\6\0\0\0\211\0\0\0\242\0\0\0b((([^^^\323\377\377\377\377\343\343\343"
  "\367\364\364\364\374\377\377\377\377\364\364\364\374\342\342\342\367"
  "\377\377\377\377\377\377\377\377ccc\333\217\217\217\344\377\377\377\377"
  "\335\335\335\365\0\0\0\302\0\0\0\40\0\0\0\5\0\0\0\1\0\0\0\3\0\0\0\34"
  "\0\0\0\204\246\246\246\332\271\271\271\352www\266\0\0\0\227666\312\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\320\320\320"
  "\363\334\334\334\366\377\377\377\377\335\335\335\365\0\0\0\302\0\0\0"
  "\40\0\0\0\4\0\0\0\0\0\0\0\2\0\0\0'\0\0\0\303\335\335\335\365\377\377"
  "\377\377\313\313\313\360eee\327555\315\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\335\335"
  "\335\365\205\205\205\313\0\0\0b\0\0\0\24\0\0\0\4\0\0\0\0\0\0\0\3\0\0"
  "\0\35\0\0\0\205\245\245\245\332\351\351\351\371\377\377\377\377\321\321"
  "\321\363\201\201\201\340\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\271\271\271\352\0\0"
  "\0\241\0\0\0\1\0\0\0\5\0\0\0\3\0\0\0\0\0\0\0\3\0\0\0\11\0\0\0\11\0\0"
  "\0\214\245\245\245\333\356\356\356\372\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\271\271\271\352\0\0\0\240\0\0\0\0\0\0\0\1\0"
  "\0\0\2\0\0\0\0\0\0\0\0\0\0\0\2\0\0\0\14\0\0\0\15\0\0\0\212\217\217\217"
  "\344\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\270\270"
  "\270\353\0\0\0\243\0\0\0\0\0\0\0\3\0\0\0\7\0\0\0\0\0\0\0\0\0\0\0\3\0"
  "\0\0\13\0\0\0\5\0\0\0\207\217\217\217\344\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\243\243\243\350...\234\0\0\0+\0\0\0\1\0\0\0\4\0\0\0\5\0"
  "\0\0\0\0\0\0\0\0\0\0\1\0\0\0\4\0\0\0\5\0\0\0""5JJJ\223\235\235\235\346"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\217\217\217\344\0\0\0\210\0\0\0\6\0\0\0\2\0"
  "\0\0\3\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\6\0\0\0\11\0\0\0"
  ":JJJ\221\245\245\245\346\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\266\266\266\356^^^\254\0\0\0N\0\0\0\3\0\0\0\1\0\0\0"
  "\1\0\0\0\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\5\0\0\0\7\0\0\0\12\0"
  "\0\0""8---\240\270\270\270\354\364\364\364\374\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377ccc\334\0\0\0o\0\0\0\20\0\0\0\6\0\0\0\0\0\0\0\1\0\0\0\1\0\0\0\0\0"
  "\0\0\0\0\0\0\1\0\0\0\3\0\0\0\4\0\0\0\11\0\0\0\13\0\0\0\25\0\0\0D\0\0"
  "\0\306\335\335\335\365\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377ddd\330\0\0\0b\0\0\0"
  "\4\0\0\0\11\0\0\0\1\0\0\0\2\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\1\0\0\0\5\0\0\0\16\0\0\0\25\0\0\0""2\0\0\0\310\334\334\334\366\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377ccc\332\0\0\0d\0\0\0\10\0\0\0\23\0\0\0\5\0\0"
  "\0\7\0\0\0\10\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\5"
  "\0\0\0\12\0\0\0\24\0\0\0""8111\267444\320444\321444\322444\321444\317"
  "444\320%%%a\0\0\0\26\0\0\0\3\0\0\0\3\0\0\0\1\0\0\0\10\0\0\0\10\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\7\0\0\0\10\0\0\0\12\0\0\0\23"
  "\0\0\0\32\0\0\0E\0\0\0Q\0\0\0Q\0\0\0O\0\0\0N\0\0\0R\0\0\0L\0\0\0\35\0"
  "\0\0\4\0\0\0\2\0\0\0\1\0\0\0\2\0\0\0\5\0\0\0\6"
};



/**
 * _exo_toolbars_set_drag_cursor:
 * @widget  : A #GtkWidget.
 **/
void
_exo_toolbars_set_drag_cursor (GtkWidget *widget)
{
  GdkCursor *cursor;
  GdkPixbuf *pixbuf;

  if (G_LIKELY (widget->window != NULL))
    {
      pixbuf = gdk_pixbuf_new_from_inline (-1, drag_cursor_data, FALSE, NULL);
      cursor = gdk_cursor_new_from_pixbuf (gtk_widget_get_display (widget),
                                           pixbuf, 12, 12);
      gdk_window_set_cursor (widget->window, cursor);
      g_object_unref (G_OBJECT (pixbuf));
      gdk_cursor_unref (cursor);
    }
}



/**
 * _exo_toolbars_unset_drag_cursor:
 * @widget  : A #GtkWidget.
 **/
void
_exo_toolbars_unset_drag_cursor (GtkWidget *widget)
{
  if (G_LIKELY (widget->window != NULL))
    gdk_window_set_cursor (widget->window, NULL);
}




