/* $Id$ */
/*-
 * Copyright (c) 2005 os-cillation e.K.
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

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EXO_MIME_INFO_H__
#define __EXO_MIME_INFO_H__

#include <glib-object.h>

G_BEGIN_DECLS;

typedef struct _ExoMimeInfoPrivate ExoMimeInfoPrivate;
typedef struct _ExoMimeInfoClass   ExoMimeInfoClass;
typedef struct _ExoMimeInfo        ExoMimeInfo;

#define EXO_TYPE_MIME_INFO            (exo_mime_info_get_type ())
#define EXO_MIME_INFO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_MIME_INFO, ExoMimeInfo))
#define EXO_MIME_INFO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_MIME_INFO, ExoMimeInfoClass))
#define EXO_IS_MIME_INFO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_MIME_INFO))
#define EXO_IS_MIME_INFO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_MIME_INFO))
#define EXO_MIME_INFO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_MIME_INFO, ExoMimeInfoClass))

GType exo_mime_info_get_type (void) G_GNUC_CONST;

const gchar *exo_mime_info_get_comment (ExoMimeInfo *info);
const gchar *exo_mime_info_get_name    (ExoMimeInfo *info);

G_END_DECLS;

#endif /* !__EXO_MIME_INFO_H__ */
