/* $Id: exo-uri.h,v 1.2 2004/09/17 09:48:24 bmeurer Exp $ */
/*-
 * Copyright (c) 2004  Benedikt Meurer <benny@xfce.org>
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

#ifndef __EXO_URI_H__
#define __EXO_URI_H__

#include <glib-object.h>

G_BEGIN_DECLS;

#define EXO_TYPE_URI             (exo_uri_get_type ())
#define EXO_URI(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_URI, ExoUri))
#define EXO_URI_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_URI, ExoUriClass))
#define EXO_IS_URI(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_URI))
#define EXO_IS_URI_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_URI))
#define EXO_URI_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_URI, ExoUriClass))
#define EXO_URI_ERROR            (exo_uri_error_quark ())


typedef enum   _ExoUriError  ExoUriError;
typedef struct _ExoUriClass  ExoUriClass;
typedef struct _ExoUri       ExoUri;

enum _ExoUriError
{
  EXO_URI_ERROR_INVALID,
};

enum
{
  EXO_URI_ENCODE_WITH_HOST = 0x01,  /* for 'file://<host>/... */
};

GType        exo_uri_get_type      (void)  G_GNUC_CONST;

GQuark       exo_uri_error_quark   (void)  G_GNUC_CONST;

ExoUri     *exo_uri_new           (const gchar  *identifier,
                                     GError      **error);

ExoUri     *exo_uri_parent        (const ExoUri  *uri);

ExoUri     *exo_uri_relative      (const ExoUri  *uri,
                                     const gchar    *name);

const gchar *exo_uri_get_scheme    (const ExoUri  *uri);
const gchar *exo_uri_get_host      (const ExoUri  *uri);
const gchar *exo_uri_get_path      (const ExoUri  *uri);
const gchar *exo_uri_get_md5sum    (const ExoUri  *uri);

gboolean     exo_uri_is_local      (const ExoUri  *uri);

gboolean     exo_uri_is_root       (const ExoUri  *uri);

gchar       *exo_uri_encode        (const ExoUri  *uri,
                                     guint           flags);
gchar       *exo_uri_to_utf8       (const ExoUri  *uri,
                                     guint           flags);

guint        exo_uri_hash          (const ExoUri  *uri);
gboolean     exo_uri_equal         (const ExoUri  *a,
                                     const ExoUri  *b);

#define exo_uri_ref(uri)   EXO_URI (g_object_ref (G_OBJECT (uri)))
#define exo_uri_unref(uri) g_object_unref (G_OBJECT (uri))

G_END_DECLS;

#endif /* !__EXO_URI_H__ */
