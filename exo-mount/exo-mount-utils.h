/* $Id$ */
/*-
 * Copyright (c) 2006-2007 Benedikt Meurer <benny@xfce.org>.
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

#ifndef __EXO_MOUNT_UTILS_H__
#define __EXO_MOUNT_UTILS_H__

#include <exo/exo.h>

G_BEGIN_DECLS;

void     exo_mount_utils_canonicalize_filename  (gchar       *filename) G_GNUC_INTERNAL;

gboolean exo_mount_utils_is_mounted             (const gchar *device_file,
                                                 gboolean    *readonly_return) G_GNUC_INTERNAL;

gboolean exo_mount_utils_is_same_device         (const gchar *device_file1,
                                                 const gchar *device_file2) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__EXO_MOUNT_UTILS_H__ */
