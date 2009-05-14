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

#ifndef __EXO_GENERIC_THUMBNAILER_H__
#define __EXO_GENERIC_THUMBNAILER_H__

#include <glib-object.h>

G_BEGIN_DECLS;

#define EXO_TYPE_GENERIC_THUMBNAILER            (exo_generic_thumbnailer_get_type ())
#define EXO_GENERIC_THUMBNAILER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_GENERIC_THUMBNAILER, ExoGenericThumbnailer))
#define EXO_GENERIC_THUMBNAILER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_GENERIC_THUMBNAILER, ExoGenericThumbnailerClass))
#define EXO_IS_GENERIC_THUMBNAILER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_GENERIC_THUMBNAILER))
#define EXO_IS_GENERIC_THUMBNAILER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_GENERIC_THUMBNAILER)
#define EXO_GENERIC_THUMBNAILER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_GENERIC_THUMBNAILER, ExoGenericThumbnailerClass))

typedef struct _ExoGenericThumbnailerPrivate ExoGenericThumbnailerPrivate;
typedef struct _ExoGenericThumbnailerClass   ExoGenericThumbnailerClass;
typedef struct _ExoGenericThumbnailer        ExoGenericThumbnailer;

GType                  exo_generic_thumbnailer_get_type    (void) G_GNUC_CONST;

ExoGenericThumbnailer *exo_generic_thumbnailer_get_default (void) G_GNUC_WARN_UNUSED_RESULT;
gboolean               exo_generic_thumbnailer_queue       (ExoGenericThumbnailer *thumbnailer,
                                                            const gchar          **uris,
                                                            const gchar          **mime_hints,
                                                            guint32                unqueue_handle,
                                                            guint32               *handle,
                                                            GError               **error);
gboolean               exo_generic_thumbnailer_test        (ExoGenericThumbnailer *thumbnailer,
                                                            GError               **error);

G_END_DECLS;

#endif /* !__EXO_GENERIC_THUMBNAILER_H__ */
