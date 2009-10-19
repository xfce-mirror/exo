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

#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <exo/exo-private.h>
#include <exo/exo-string.h>



void
_exo_i18n_init (void)
{
  static gboolean inited = FALSE;

  if (G_UNLIKELY (!inited))
    {
      inited = TRUE;

      bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
    }
}



void
_exo_gtk_widget_send_focus_change (GtkWidget *widget,
                                   gboolean   in)
{
  GdkEvent *fevent;

  g_object_ref (G_OBJECT (widget));

 if (in)
    GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);
  else
    GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);

  fevent = gdk_event_new (GDK_FOCUS_CHANGE);
  fevent->focus_change.type = GDK_FOCUS_CHANGE;
  fevent->focus_change.window = g_object_ref (widget->window);
  fevent->focus_change.in = in;

  gtk_widget_event (widget, fevent);

  g_object_notify (G_OBJECT (widget), "has-focus");

  g_object_unref (G_OBJECT (widget));
  gdk_event_free (fevent);
}

