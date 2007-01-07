/* $Id$ */
/*-
 * Copyright (c) 2005-2007 Benedikt Meurer <benny@xfce.org>.
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

#ifndef __EXO_HAL_H__
#define __EXO_HAL_H__

#include <glib.h>

G_BEGIN_DECLS;

/* forward declarations for libhal-storage */
#ifndef LIBHAL_STORAGE_H
struct LibHalContext_s;
struct LibHalVolume_s;
struct LibHalDrive_s;
#endif

/* verify that G_GNUC_WARN_UNUSED_RESULT is defined */
#if !defined(G_GNUC_WARN_UNUSED_RESULT)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define G_GNUC_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define G_GNUC_WARN_UNUSED_RESULT
#endif /* __GNUC__ */
#endif /* !defined(G_GNUC_WARN_UNUSED_RESULT) */

gboolean exo_hal_init                         (void);

gchar   *exo_hal_drive_compute_display_name   (struct LibHalContext_s *context,
                                               struct LibHalDrive_s   *drive) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
GList   *exo_hal_drive_compute_icon_list      (struct LibHalContext_s *context,
                                               struct LibHalDrive_s   *drive) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gchar   *exo_hal_volume_compute_display_name  (struct LibHalContext_s *context,
                                               struct LibHalVolume_s  *volume,
                                               struct LibHalDrive_s   *drive) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
GList   *exo_hal_volume_compute_icon_list     (struct LibHalContext_s *context,
                                               struct LibHalVolume_s  *volume,
                                               struct LibHalDrive_s   *drive) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS;

#endif /* !__EXO_HAL_H__ */
