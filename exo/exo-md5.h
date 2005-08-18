/* $Id$ */
/*-
 * Copyright (c) 2004 os-cillation e.K.
 * Copyright (c) 2004 James M. Cape <jcape@ignore-your.tv>
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

#ifndef __EXO_MD5_H__
#define __EXO_MD5_H__

#include <exo/exo-config.h>
#include <glib-object.h>

G_BEGIN_DECLS;

#define EXO_TYPE_MD5_DIGEST (exo_md5_digest_get_type ())

typedef struct _ExoMd5Digest ExoMd5Digest;
struct _ExoMd5Digest
{
  guchar digest[16];
};

GType         exo_md5_digest_get_type (void)  G_GNUC_CONST;

ExoMd5Digest *exo_str_get_md5_digest  (const gchar        *contents);
gchar        *exo_str_get_md5_str     (const gchar        *contents);

ExoMd5Digest *exo_md5_str_to_digest   (const gchar        *str_digest);
gchar        *exo_md5_digest_to_str   (const ExoMd5Digest *digest);

ExoMd5Digest *exo_md5_digest_dup      (const ExoMd5Digest *digest);
void          exo_md5_digest_free     (ExoMd5Digest       *digest);

guint         exo_md5_digest_hash     (gconstpointer       digest);
gboolean      exo_md5_digest_equal    (gconstpointer       digest1,
                                       gconstpointer       digest2);

G_END_DECLS;

#endif /* !__EXO_MD5_H__ */
