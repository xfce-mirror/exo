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

#ifndef __EXO_THUMBNAILER_SERVICE_H__
#define __EXO_THUMBNAILER_SERVICE_H__

#include <glib-object.h>

G_BEGIN_DECLS;

#define EXO_TYPE_THUMBNAILER_SERVICE            (exo_thumbnailer_service_get_type ())
#define EXO_THUMBNAILER_SERVICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_THUMBNAILER_SERVICE, ExoThumbnailerService))
#define EXO_THUMBNAILER_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_THUMBNAILER_SERVICE, ExoThumbnailerServiceClass))
#define EXO_IS_THUMBNAILER_SERVICE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_THUMBNAILER_SERVICE))
#define EXO_IS_THUMBNAILER_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_THUMBNAILER_SERVICE)
#define EXO_THUMBNAILER_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_THUMBNAILER_SERVICE, ExoThumbnailerServiceClass))

typedef struct _ExoThumbnailerServicePrivate ExoThumbnailerServicePrivate;
typedef struct _ExoThumbnailerServiceClass   ExoThumbnailerServiceClass;
typedef struct _ExoThumbnailerService        ExoThumbnailerService;

GType                  exo_thumbnailer_service_get_type   (void) G_GNUC_CONST;

ExoThumbnailerService *exo_thumbnailer_service_new_unique (GError **error) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS;

#endif /* !__EXO_THUMBNAILER_SERVICE_H__ */
