/* $Id$ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>.
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

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EXO_URL_H__
#define __EXO_URL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS;

#define EXO_URL_ERROR (exo_url_error_quark ())
GQuark exo_url_error_quark (void) G_GNUC_CONST;

/**
 * ExoUrlError:
 * @EXO_URL_ERROR_NOT_SUPPORTED : a given URL is not supported.
 * 
 * The errors that can be returned due to bad parameters being
 * passed to exo_url_show() or exo_url_show_on_screen().
 **/
typedef enum /*< skip >*/
{
  EXO_URL_ERROR_NOT_SUPPORTED,
} ExoUrlError;

gboolean exo_url_show               (const gchar    *url,
                                     gchar         **envp,
                                     GError        **error);

gboolean exo_url_show_on_screen     (const gchar    *url,
                                     gchar         **envp,
                                     GdkScreen      *screen,
                                     GError        **error);

void     exo_url_about_dialog_hook  (GtkAboutDialog *about_dialog,
                                     const gchar    *link,
                                     gpointer        user_data);

G_END_DECLS;

#endif /* !__EXO_URL_H__ */
