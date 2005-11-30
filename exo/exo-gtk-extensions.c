/* $Id$ */
/*-
 * Copyright (c) 2004 os-cillation e.K.
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
 * @object  :
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
 * @current_value :
 **/
void
exo_gtk_radio_action_set_current_value (GtkRadioAction *action,
                                        gint            current_value)
{
  GSList *lp;
  gint    value;

  g_return_if_fail (GTK_IS_RADIO_ACTION (action));

  for (lp = gtk_radio_action_get_group (action); lp != NULL; lp = lp->next)
    {
      g_object_get (G_OBJECT (lp->data), "value", &value, NULL);
      if (value == current_value)
        {
          gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (lp->data), TRUE);
          break;
        }
    }
}



#define __EXO_GTK_EXTENSIONS_C__
#include <exo/exo-aliasdef.c>
