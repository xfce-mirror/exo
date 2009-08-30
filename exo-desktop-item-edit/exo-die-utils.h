/* $Id$ */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>.
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

#ifndef __EXO_DIE_UTILS_H__
#define __EXO_DIE_UTILS_H__

#include <exo/exo.h>
#include <gio/gio.h>

G_BEGIN_DECLS

void      exo_die_g_key_file_set_locale_value (GKeyFile    *key_file,
                                               const gchar *group,
                                               const gchar *key,
                                               const gchar *value);

gboolean  exo_die_g_key_file_save             (GKeyFile    *key_file,
                                               gboolean     create,
                                               GFile       *base,
                                               GError     **error);

G_END_DECLS

#endif /* !__EXO_DIE_UTILS_H__ */
