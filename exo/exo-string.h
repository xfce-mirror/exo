/* $Id$ */
/*-
 * Copyright (c) 2004-2007 os-cillation e.K.
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

#ifndef __EXO_STRING_H__
#define __EXO_STRING_H__

#include <exo/exo-config.h>

G_BEGIN_DECLS

gchar                *exo_str_elide_underscores  (const gchar     *text) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gboolean              exo_str_is_equal           (const gchar     *a,
                                                  const gchar     *b);

gchar                *exo_str_replace            (const gchar     *str,
                                                  const gchar     *pattern,
                                                  const gchar     *replacement) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gchar                *exo_strdup_strftime        (const gchar     *format,
                                                  const struct tm *tm) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gchar               **exo_strndupv               (gchar          **strv,
                                                  guint            num) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

#define I_(string) (g_intern_static_string ((string)))

#define exo_str_is_empty(string) ((string) == NULL || *(string) == '\0')

G_END_DECLS

#endif /* !__EXO_STRING_H__ */
