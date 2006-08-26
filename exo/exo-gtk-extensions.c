/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
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

#include <exo/exo-gtk-extensions.h>
#include <exo/exo-private.h>
#include <exo/exo-thumbnail-preview.h>
#include <exo/exo-alias.h>



static gboolean
later_destroy (gpointer object)
{
  gtk_object_destroy (GTK_OBJECT (object));
  g_object_unref (G_OBJECT (object));
  return FALSE;
}



/**
 * exo_gtk_object_destroy_later:
 * @object : a #GtkObject.
 *
 * Schedules an idle function to destroy the specified @object
 * when the application enters the main loop the next time.
 **/
void
exo_gtk_object_destroy_later (GtkObject *object)
{
  g_return_if_fail (GTK_IS_OBJECT (object));

  g_idle_add_full (G_PRIORITY_HIGH, later_destroy, object, NULL);
  exo_gtk_object_ref_sink (object);
}



/**
 * exo_gtk_object_ref_sink:
 * @object : a #GtkObject.
 *
 * Helper function used to take a reference on
 * @object and droppping the floating reference
 * to @object (if any) atomically.
 *
 * If libexo is compiled against Gtk+ 2.9.0 or
 * newer, this function will use g_object_ref_sink(),
 * since with newer Gtk+/GObject versions, the floating
 * reference handling was moved to GObject. Else, this
 * function will expand to
 *
 * <informalexample><programlisting>
 * g_object_ref (G_OBJECT (object));
 * gtk_object_sink (GTK_OBJECT (object));
 * </programlisting></informalexample>
 *
 * The caller is responsible to release the reference
 * on @object acquire by this function call using
 * g_object_unref().
 *
 * Return value: a reference to @object.
 **/
gpointer
exo_gtk_object_ref_sink (GtkObject *object)
{
  g_return_val_if_fail (GTK_IS_OBJECT (object), NULL);

#if GTK_CHECK_VERSION(2,9,0)
  g_object_ref_sink (G_OBJECT (object));
#else
  g_object_ref (G_OBJECT (object));
  gtk_object_sink (object);
#endif

  return object;
}



/**
 * exo_gtk_radio_action_set_current_value:
 * @action        : A #GtkRadioAction.
 * @current_value : the value of the #GtkRadioAction to activate.
 *
 * Looks for all actions in the group to which @action belongs and if
 * any of the actions matches the @current_value, it will become the
 * new active action.
 *
 * Else if none of the actions in @action<!---->'s radio group match
 * the specified @current_value, all actions will be deactivated and
 * the radio group will have no active action afterwards.
 **/
void
exo_gtk_radio_action_set_current_value (GtkRadioAction *action,
                                        gint            current_value)
{
  GSList *lp;
  gint    value;

  g_return_if_fail (GTK_IS_RADIO_ACTION (action));

  /* check if we have action who's value matches */
  for (lp = gtk_radio_action_get_group (action); lp != NULL; lp = lp->next)
    {
      g_object_get (G_OBJECT (lp->data), "value", &value, NULL);
      if (value == current_value)
        {
          gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (lp->data), TRUE);
          return;
        }
    }

  /* no action found, so none of the actions gets the "active" flag */
  for (lp = gtk_radio_action_get_group (action); lp != NULL; lp = lp->next)
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (lp->data), FALSE);
}



static void
update_preview (GtkFileChooser      *chooser,
                ExoThumbnailPreview *thumbnail_preview)
{
  gchar *uri;

  _exo_return_if_fail (EXO_IS_THUMBNAIL_PREVIEW (thumbnail_preview));
  _exo_return_if_fail (GTK_IS_FILE_CHOOSER (chooser));

  /* update the URI for the preview */
  uri = gtk_file_chooser_get_preview_uri (chooser);
  if (G_UNLIKELY (uri == NULL))
    {
      /* gee, why is there a get_preview_uri() method if
       * it doesn't work in several cases? did anybody ever
       * test this method prior to committing it?
       */
      uri = gtk_file_chooser_get_uri (chooser);
    }
  _exo_thumbnail_preview_set_uri (thumbnail_preview, uri);
  g_free (uri);
}



/**
 * exo_gtk_file_chooser_add_thumbnail_preview:
 * @chooser : a #GtkFileChooser.
 *
 * This is a convenience function that adds a preview widget to the @chooser,
 * which displays thumbnails for the selected filenames using the thumbnail
 * database. The preview widget is also able to generate thumbnails for all
 * image formats supported by #GdkPixbuf.
 *
 * Use this function whenever you display a #GtkFileChooser to ask the user
 * to select an image file from the file system.
 *
 * The preview widget also supports URIs other than file:-URIs to a certain
 * degree, but this support is rather limited currently, so you may want to
 * use gtk_file_chooser_set_local_only() to ensure that the user can only
 * select files from the local file system.
 *
 * When @chooser is configured to select multiple image files - using the
 * gtk_file_chooser_set_select_multiple() method - the behaviour of the
 * preview widget is currently undefined, in that it is not defined for
 * which of the selected files the preview will be displayed.
 *
 * Since: 0.3.1.9
 **/
void
exo_gtk_file_chooser_add_thumbnail_preview (GtkFileChooser *chooser)
{
  GtkWidget *thumbnail_preview;

  g_return_if_fail (GTK_IS_FILE_CHOOSER (chooser));

  /* add the preview to the file chooser */
  thumbnail_preview = _exo_thumbnail_preview_new ();
  gtk_file_chooser_set_preview_widget (chooser, thumbnail_preview);
  gtk_file_chooser_set_preview_widget_active (chooser, TRUE);
  gtk_file_chooser_set_use_preview_label (chooser, FALSE);
  gtk_widget_show (thumbnail_preview);

  /* update the preview as necessary */
  g_signal_connect_object (G_OBJECT (chooser), "selection-changed", G_CALLBACK (update_preview), thumbnail_preview, 0);

  /* initially update the preview, in case the file chooser is already setup */
  update_preview (chooser, EXO_THUMBNAIL_PREVIEW (thumbnail_preview));
}



#define __EXO_GTK_EXTENSIONS_C__
#include <exo/exo-aliasdef.c>
