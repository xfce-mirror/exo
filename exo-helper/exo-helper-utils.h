/* $Id$ */
/*-
 * Copyright (c) 2003-2006 Benedikt Meurer <benny@xfce.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef __EXO_HELPER_UTILS_H__
#define __EXO_HELPER_UTILS_H__

#include <exo-helper/exo-helper.h>

G_BEGIN_DECLS;

gboolean   exo_helper_category_from_string  (const gchar       *string,
                                             ExoHelperCategory *category_return) G_GNUC_INTERNAL;
gchar     *exo_helper_category_to_string    (ExoHelperCategory  category) G_GNUC_INTERNAL G_GNUC_MALLOC;

GtkWidget *exo_helper_create_header         (const gchar       *icon,
                                             const gchar       *text) G_GNUC_INTERNAL G_GNUC_MALLOC;

G_END_DECLS;

#endif /* !__EXO_HELPER_UTILS_H__ */
