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

#ifndef __EXO_MOUNT_HAL_H__
#define __EXO_MOUNT_HAL_H__

#include <exo/exo.h>

G_BEGIN_DECLS

typedef struct _ExoMountHalDevice ExoMountHalDevice;

ExoMountHalDevice *exo_mount_hal_device_from_udi    (const gchar       *udi,
                                                     GError           **error) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
ExoMountHalDevice *exo_mount_hal_device_from_file   (const gchar       *file,
                                                     GError           **error) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

void               exo_mount_hal_device_free        (ExoMountHalDevice *device);

gchar             *exo_mount_hal_device_get_file    (ExoMountHalDevice *device) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gchar             *exo_mount_hal_device_get_name    (ExoMountHalDevice *device) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gchar             *exo_mount_hal_device_get_icon    (ExoMountHalDevice *device) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gboolean           exo_mount_hal_device_is_readonly (ExoMountHalDevice *device);

gboolean           exo_mount_hal_device_eject       (ExoMountHalDevice *device,
                                                     GError           **error);
gboolean           exo_mount_hal_device_mount       (ExoMountHalDevice *device,
                                                     GError           **error);
gboolean           exo_mount_hal_device_unmount     (ExoMountHalDevice *device,
                                                     GError           **error);

G_END_DECLS

#endif /* !__EXO_MOUNT_HAL_H__ */
