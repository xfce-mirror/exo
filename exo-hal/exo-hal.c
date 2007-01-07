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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBINTL_h
#include <libintl.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef HAVE_HAL
#include <libhal-storage.h>
#endif

#include <glib/gi18n-lib.h>

#include <exo-hal/exo-hal.h>



#ifdef HAVE_HAL
/* HAL icon mappings, borrowed from gnome-vfs-hal-mounts.c (Revision 5187) */
typedef enum
{
  HAL_ICON_DRIVE_REMOVABLE_DISK           = 0x10000,
  HAL_ICON_DRIVE_REMOVABLE_DISK_IDE       = 0x10001,
  HAL_ICON_DRIVE_REMOVABLE_DISK_SCSI      = 0x10002,
  HAL_ICON_DRIVE_REMOVABLE_DISK_USB       = 0x10003,
  HAL_ICON_DRIVE_REMOVABLE_DISK_IEEE1394  = 0x10004,
  HAL_ICON_DRIVE_REMOVABLE_DISK_CCW       = 0x10005,
  HAL_ICON_DRIVE_DISK                     = 0x10100,
  HAL_ICON_DRIVE_DISK_IDE                 = 0x10101,
  HAL_ICON_DRIVE_DISK_SCSI                = 0x10102,
  HAL_ICON_DRIVE_DISK_USB                 = 0x10103,
  HAL_ICON_DRIVE_DISK_IEEE1394            = 0x10104,
  HAL_ICON_DRIVE_DISK_CCW                 = 0x10105,
  HAL_ICON_DRIVE_CDROM                    = 0x10200,
  HAL_ICON_DRIVE_CDWRITER                 = 0x102ff,
  HAL_ICON_DRIVE_FLOPPY                   = 0x10300,
  HAL_ICON_DRIVE_TAPE                     = 0x10400,
  HAL_ICON_DRIVE_COMPACT_FLASH            = 0x10500,
  HAL_ICON_DRIVE_MEMORY_STICK             = 0x10600,
  HAL_ICON_DRIVE_SMART_MEDIA              = 0x10700,
  HAL_ICON_DRIVE_SD_MMC                   = 0x10800,
  HAL_ICON_DRIVE_CAMERA                   = 0x10900,
  HAL_ICON_DRIVE_PORTABLE_AUDIO_PLAYER    = 0x10a00,
  HAL_ICON_DRIVE_ZIP                      = 0x10b00,
  HAL_ICON_DRIVE_JAZ                      = 0x10c00,
  HAL_ICON_DRIVE_FLASH_KEY                = 0x10d00,

  HAL_ICON_VOLUME_REMOVABLE_DISK          = 0x20000,
  HAL_ICON_VOLUME_REMOVABLE_DISK_IDE      = 0x20001,
  HAL_ICON_VOLUME_REMOVABLE_DISK_SCSI     = 0x20002,
  HAL_ICON_VOLUME_REMOVABLE_DISK_USB      = 0x20003,
  HAL_ICON_VOLUME_REMOVABLE_DISK_IEEE1394 = 0x20004,
  HAL_ICON_VOLUME_REMOVABLE_DISK_CCW      = 0x20005,
  HAL_ICON_VOLUME_DISK                    = 0x20100,
  HAL_ICON_VOLUME_DISK_IDE                = 0x20101,
  HAL_ICON_VOLUME_DISK_SCSI               = 0x20102,
  HAL_ICON_VOLUME_DISK_USB                = 0x20103,
  HAL_ICON_VOLUME_DISK_IEEE1394           = 0x20104,
  HAL_ICON_VOLUME_DISK_CCW                = 0x20105,
  /* specifically left out as we use icons based on media type in the optical drive
  HAL_ICON_VOLUME_CDROM                   = 0x20200 */
  HAL_ICON_VOLUME_FLOPPY                  = 0x20300,
  HAL_ICON_VOLUME_TAPE                    = 0x20400,
  HAL_ICON_VOLUME_COMPACT_FLASH           = 0x20500,
  HAL_ICON_VOLUME_MEMORY_STICK            = 0x20600,
  HAL_ICON_VOLUME_SMART_MEDIA             = 0x20700,
  HAL_ICON_VOLUME_SD_MMC                  = 0x20800,
  HAL_ICON_VOLUME_CAMERA                  = 0x20900,
  HAL_ICON_VOLUME_PORTABLE_AUDIO_PLAYER   = 0x20a00,
  HAL_ICON_VOLUME_ZIP                     = 0x20b00,
  HAL_ICON_VOLUME_JAZ                     = 0x20c00,
  HAL_ICON_VOLUME_FLASH_KEY               = 0x20d00,

  HAL_ICON_DISC_CDROM                     = 0x30000,
  HAL_ICON_DISC_CDR                       = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_CDR,
  HAL_ICON_DISC_CDRW                      = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_CDRW,
  HAL_ICON_DISC_DVDROM                    = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_DVDROM,
  HAL_ICON_DISC_DVDRAM                    = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_DVDRAM,
  HAL_ICON_DISC_DVDR                      = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_DVDR,
  HAL_ICON_DISC_DVDRW                     = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_DVDRW,
  HAL_ICON_DISC_DVDPLUSR                  = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_DVDPLUSR,
  HAL_ICON_DISC_DVDPLUSRW                 = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_DVDPLUSRW,
  HAL_ICON_DISC_DVDPLUSR_DL               = HAL_ICON_DISC_CDROM + LIBHAL_VOLUME_DISC_TYPE_DVDPLUSR_DL,
} HalIcon;

typedef struct
{
  HalIcon     icon;
  const gchar name[25];
} HalIconPair;

/* by design, the enums are laid out so we can do easy computations */
static const HalIconPair hal_icon_mapping[] =
{
#if 0 /* gnome-dev-removable is the fallback */
  { HAL_ICON_DRIVE_REMOVABLE_DISK,           "gnome-dev-removable" },
  { HAL_ICON_DRIVE_REMOVABLE_DISK_IDE,       "gnome-dev-removable" },
  { HAL_ICON_DRIVE_REMOVABLE_DISK_SCSI,      "gnome-dev-removable" },
#endif
  { HAL_ICON_DRIVE_REMOVABLE_DISK_USB,       "gnome-dev-removable-usb" },
  { HAL_ICON_DRIVE_REMOVABLE_DISK_IEEE1394,  "gnome-dev-removable-1394" },
#if 0 /* gnome-dev-removable is the fallback */
  { HAL_ICON_DRIVE_REMOVABLE_DISK_CCW,       "gnome-dev-removable" },
  { HAL_ICON_DRIVE_DISK,                     "gnome-dev-removable" },
  { HAL_ICON_DRIVE_DISK_IDE,                 "gnome-dev-removable" },
  { HAL_ICON_DRIVE_DISK_SCSI,                "gnome-dev-removable" },       /* TODO: gnome-dev-removable-scsi */
#endif
  { HAL_ICON_DRIVE_DISK_USB,                 "gnome-dev-removable-usb" },
  { HAL_ICON_DRIVE_DISK_IEEE1394,            "gnome-dev-removable-1394" },
#if 0 /* gnome-dev-removable is the fallback */
  { HAL_ICON_DRIVE_DISK_CCW,                 "gnome-dev-removable" },
  { HAL_ICON_DRIVE_CDROM,                    "gnome-dev-removable" },       /* TODO: gnome-dev-removable-cdrom */
  { HAL_ICON_DRIVE_CDWRITER,                 "gnome-dev-removable" },       /* TODO: gnome-dev-removable-cdwriter */
#endif
  { HAL_ICON_DRIVE_FLOPPY,                   "gnome-dev-floppy" },
#if 0 /* gnome-dev-removable is the fallback */
  { HAL_ICON_DRIVE_TAPE,                     "gnome-dev-removable" },       /* TODO: gnome-dev-removable-tape */
#endif
  { HAL_ICON_DRIVE_COMPACT_FLASH,            "gnome-dev-media-cf" },
  { HAL_ICON_DRIVE_MEMORY_STICK,             "gnome-dev-media-ms" },
  { HAL_ICON_DRIVE_SMART_MEDIA,              "gnome-dev-media-sm" },
  { HAL_ICON_DRIVE_SD_MMC,                   "gnome-dev-media-sdmmc" },
  { HAL_ICON_DRIVE_CAMERA,                   "camera-photo" },
  { HAL_ICON_DRIVE_PORTABLE_AUDIO_PLAYER,    "gnome-dev-ipod" },
  { HAL_ICON_DRIVE_ZIP,                      "gnome-dev-zipdisk" },
  { HAL_ICON_DRIVE_JAZ,                      "gnome-dev-jazdisk" },
#if 0 /* gnome-dev-removable is the fallback */
  { HAL_ICON_DRIVE_FLASH_KEY,                "gnome-dev-removable" },       /* TODO: gnome-dev-removable-pendrive */
#endif

  { HAL_ICON_VOLUME_REMOVABLE_DISK,          "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_REMOVABLE_DISK_IDE,      "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_REMOVABLE_DISK_SCSI,     "gnome-dev-harddisk" },        /* TODO: gnome-dev-harddisk-scsi */
  { HAL_ICON_VOLUME_REMOVABLE_DISK_USB,      "gnome-dev-harddisk-usb" },
  { HAL_ICON_VOLUME_REMOVABLE_DISK_IEEE1394, "gnome-dev-harddisk-1394" },
  { HAL_ICON_VOLUME_REMOVABLE_DISK_CCW,      "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_DISK,                    "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_DISK_IDE,                "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_DISK_SCSI,               "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_DISK_USB,                "gnome-dev-harddisk-usb" },
  { HAL_ICON_VOLUME_DISK_IEEE1394,           "gnome-dev-harddisk-1394" },
  { HAL_ICON_VOLUME_DISK_CCW,                "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_FLOPPY,                  "gnome-dev-floppy" },
  { HAL_ICON_VOLUME_TAPE,                    "gnome-dev-harddisk" },
  { HAL_ICON_VOLUME_COMPACT_FLASH,           "gnome-dev-media-cf" },
  { HAL_ICON_VOLUME_MEMORY_STICK,            "gnome-dev-media-ms" },
  { HAL_ICON_VOLUME_SMART_MEDIA,             "gnome-dev-media-sm" },
  { HAL_ICON_VOLUME_SD_MMC,                  "gnome-dev-media-sdmmc" },
  { HAL_ICON_VOLUME_CAMERA,                  "camera-photo" },
  { HAL_ICON_VOLUME_PORTABLE_AUDIO_PLAYER,   "gnome-dev-ipod" },
  { HAL_ICON_VOLUME_ZIP,                     "gnome-dev-zipdisk" },
  { HAL_ICON_VOLUME_JAZ,                     "gnome-dev-jazdisk" },
  { HAL_ICON_VOLUME_FLASH_KEY,               "gnome-dev-harddisk" },        /* TODO: gnome-dev-pendrive */

  { HAL_ICON_DISC_CDROM,                     "gnome-dev-cdrom" },
  { HAL_ICON_DISC_CDR,                       "gnome-dev-disc-cdr" },
  { HAL_ICON_DISC_CDRW,                      "gnome-dev-disc-cdrw" },
  { HAL_ICON_DISC_DVDROM,                    "gnome-dev-disc-dvdrom" },
  { HAL_ICON_DISC_DVDRAM,                    "gnome-dev-disc-dvdram" },
  { HAL_ICON_DISC_DVDR,                      "gnome-dev-disc-dvdr" },
  { HAL_ICON_DISC_DVDRW,                     "gnome-dev-disc-dvdrw" },
  { HAL_ICON_DISC_DVDPLUSR,                  "gnome-dev-disc-dvdr-plus" },
  { HAL_ICON_DISC_DVDPLUSRW,                 "gnome-dev-disc-dvdrw" },      /* TODO: gnome-dev-disc-dvdrw-plus */
  { HAL_ICON_DISC_DVDPLUSR_DL,               "gnome-dev-disc-dvdr-plus" },  /* TODO: gnome-dev-disc-dvdr-plus-dl */
};

static const gchar*
exo_hal_lookup_icon_name (HalIcon icon)
{
  guint n;

  for (n = 0; n < G_N_ELEMENTS (hal_icon_mapping); ++n)
    if (hal_icon_mapping[n].icon == icon)
      return hal_icon_mapping[n].name;

  return NULL;
}
#endif



/**
 * exo_hal_init:
 *
 * Initializes the HAL support module, which includes setting up the
 * internationalization support. Returns %TRUE if support for HAL was
 * enabled at compile time, %FALSE otherwise.
 *
 * Make sure you call this function first prior to calling any of the
 * functions below.
 *
 * Return value: %TRUE if HAL support is available, %FALSE otherwise.
 *
 * Since: 0.3.1.13
 **/
gboolean
exo_hal_init (void)
{
#ifdef HAVE_HAL
  /* setup the i18n support */
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

  /* tell the caller that we generally support HAL */
  return TRUE;
#else
  /* tell the caller that we do not support HAL */
  return FALSE;
#endif
}



/**
 * exo_hal_drive_compute_display_name:
 * @context : a #LibHalContext, connected to the HAL daemon.
 * @drive   : a #LibHalDrive.
 *
 * Computes a usable display name that should be used to present
 * @drive to the user. May return %NULL if it's unable to determine
 * a display name (i.e. if HAL support is not available), in which
 * case the caller should try to come up with a fallback name on
 * it's own (i.e. using the basename of the @drive<!---->s device
 * file or something like that).
 *
 * The caller is responsible to free the returned string using
 * g_free() when no longer needed.
 *
 * Return value: a display name for the @drive that should be used
 *               to present the @drive to the user or %NULL if the
 *               function is unable to come up with a usable name
 *               and the caller should figure out a fallback name
 *               on its own.
 *
 * Since: 0.3.1.13
 **/
gchar*
exo_hal_drive_compute_display_name (struct LibHalContext_s *context,
                                    struct LibHalDrive_s   *drive)
{
#ifdef HAVE_HAL
  LibHalDriveCdromCaps cdrom_caps;
  const gchar         *vendor;
  const gchar         *model;
  const gchar         *second;
  const gchar         *first;
  gchar               *display_name;
  gchar               *name;

  g_return_val_if_fail (context != NULL, NULL);
  g_return_val_if_fail (drive != NULL, NULL);

  /* determine the vendor and the model */
  vendor = libhal_drive_get_vendor (drive);
  model = libhal_drive_get_model (drive);

  /* display name depends on the drive type */
  switch (libhal_drive_get_type (drive))
    {
    case LIBHAL_DRIVE_TYPE_CDROM:
      /* determine the capabilities of the CD-ROM drive */
      cdrom_caps = libhal_drive_get_cdrom_caps (drive);

      /* determine the first capability of the drive */
      if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_CDRW) != 0)
        first = "CD-RW";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_CDR) != 0)
        first = "CD-R";
      else
        first = "CD-ROM";

      /* determine the second capability of the drive (if any) */
      if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_HDDVDRW) != 0)
        second = "/HD DVD-RW";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_HDDVDR) != 0)
        second = "/HD DVD-R";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_HDDVDROM) != 0)
        second = "/HD DVD-ROM";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_BDRE) != 0)
        second = "/BD-RE";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_BDR) != 0)
        second = "/BD-R";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_BDROM) != 0)
        second = "/BD-ROM";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDRW) != 0
            && (cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDPLUSRW) != 0)
        {
          if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDPLUSRDL) != 0 || (cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDPLUSRWDL) != 0)
            second = "/DVD±RW DL";
          else
            second = "/DVD±RW";
        }
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDR) != 0
            && (cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDPLUSR) != 0)
        {
          if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDPLUSRDL) != 0)
            second = "/DVD±R DL";
          else
            second = "/DVD±R";
        }
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDRAM) != 0)
        second = "/DVD-RAM";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDRW) != 0)
        second = "/DVD-RW";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDR) != 0)
        second = "/DVD-R";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDPLUSRW) != 0)
        second = "/DVD+RW";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDPLUSR) != 0)
        second = "/DVD+R";
      else if ((cdrom_caps & LIBHAL_DRIVE_CDROM_CAPS_DVDROM) != 0)
        second = "/DVD-ROM";
      else
        second = "";

      /* combine the capabilities */
      name = g_strconcat (first, second, NULL);

      /* now the exact name depends on whether the drive is hotpluggable */
      if (G_UNLIKELY (libhal_drive_is_hotpluggable (drive)))
        display_name = g_strdup_printf (_("External %s Drive"), name);
      else
        display_name = g_strdup_printf (_("%s Drive"), name);

      /* cleanup */
      g_free (name);
      break;

    case LIBHAL_DRIVE_TYPE_FLOPPY:
      /* we support both internal and external floppy drives */
      if (G_UNLIKELY (libhal_drive_is_hotpluggable (drive)))
        display_name = g_strdup (_("External Floppy Drive"));
      else
        display_name = g_strdup (_("Floppy Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_COMPACT_FLASH:
      display_name = g_strdup (_("Compact Flash Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_MEMORY_STICK:
      display_name = g_strdup (_("Memory Stick Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_SMART_MEDIA:
      display_name = g_strdup (_("Smart Media Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_SD_MMC:
      display_name = g_strdup (_("SD/MMC Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_ZIP:
      display_name = g_strdup (_("Zip Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_JAZ:
      display_name = g_strdup (_("Jaz Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_FLASHKEY:
      display_name = g_strdup (_("Pen Drive"));
      break;

    case LIBHAL_DRIVE_TYPE_PORTABLE_AUDIO_PLAYER:
      /* combine the vendor and model (dropping any redundant whitespace) */
      name = g_strdup_printf ("%s %s", (vendor != NULL) ? vendor : "", (model != NULL) ? model : "");
      g_strstrip (name);

      /* TRANSLATORS: This string requires special care as %s may be the empty string. Trailing/leading whitespace will be removed. */
      display_name = g_strdup_printf (_("%s Music Player"), name);

      /* strip the display name */
      g_strstrip (display_name);

      /* cleanup */
      g_free (name);
      break;

    case LIBHAL_DRIVE_TYPE_CAMERA:
      /* combine the vendor and model (dropping any redundant whitespace) */
      name = g_strdup_printf ("%s %s", (vendor != NULL) ? vendor : "", (model != NULL) ? model : "");
      g_strstrip (name);

      /* TRANSLATORS: This string requires special care as %s may be the empty string. Trailing/leading whitespace will be removed. */
      display_name = g_strdup_printf (_("%s Digital Camera"), name);

      /* strip the display name */
      g_strstrip (display_name);

      /* cleanup */
      g_free (name);
      break;

    default:
      /* generate a display name from the vendor and the model */
      display_name = g_strdup_printf ("%s %s", (vendor != NULL) ? vendor : "", (model != NULL) ? model : "");

      /* drop additional whitespace */
      g_strstrip (display_name);

      /* check if we still don't have a name */
      if (G_UNLIKELY (*display_name == '\0'))
        {
          /* cleanup */
          g_free (display_name);

          /* last fallback to "Drive" */
          display_name = g_strdup (_("Drive"));
        }
      break;
    }

  return display_name;
#else
  /* HAL is not available, impossible to figure out
   * a usable display name for the given drive...
   */
  return NULL;
#endif
}



/**
 * exo_hal_drive_compute_icon_list:
 * @context    : a #LibHalContext, connected to the HAL daemon.
 * @drive      : a #LibHalDrive.
 *
 * Tries to find a list of icon names that may be used to visually present @drive
 * to the user. The list is sorted by relevance, with the best icon matches
 * appearing first in the list.
 *
 * The caller is responsible to free the returned list using
 * <informalexample><programlisting>
 * g_list_foreach (list, (GFunc) g_free, NULL);
 * g_list_free (list);
 * </programlisting></informalexample>
 * when no longer needed.
 *
 * Return value: a list of icon names for icons that can be used to visually
 *               represent the @drive to the user.
 *
 * Since: 0.3.1.13
 **/
GList*
exo_hal_drive_compute_icon_list (struct LibHalContext_s *context,
                                 struct LibHalDrive_s   *drive)
{
  GList               *icon_list = NULL;

#ifdef HAVE_HAL
  LibHalDriveCdromCaps cdrom_caps;
  LibHalDriveType      type;
  LibHalDriveBus       bus;
  const gchar         *icon_name;

  g_return_val_if_fail (context != NULL, NULL);
  g_return_val_if_fail (drive != NULL, NULL);

  /* check if a dedicated drive icon name is known */
  icon_name = libhal_drive_get_dedicated_icon_drive (drive);
  if (G_UNLIKELY (icon_name != NULL))
    icon_list = g_list_append (icon_list, g_strdup (icon_name));

  /* determine the type and bus of the drive */
  type = libhal_drive_get_type (drive);
  bus = libhal_drive_get_bus (drive);

  /* lookup depends on the drive type */
  switch (type)
    {
    case LIBHAL_DRIVE_TYPE_CDROM:
      /* determine the CD-ROM capabilities */
      cdrom_caps = libhal_drive_get_cdrom_caps (drive);

      /* check if this is a CD/DVD writer */
      if ((cdrom_caps & (LIBHAL_DRIVE_CDROM_CAPS_CDROM | LIBHAL_DRIVE_CDROM_CAPS_DVDROM)) != cdrom_caps)
        {
          /* check if we have a specific writer icon here */
          icon_name = exo_hal_lookup_icon_name (0x10000 + type * 0x100 + 0xff);
          if (G_LIKELY (icon_name != NULL))
            icon_list = g_list_append (icon_list, g_strdup (icon_name));
        }
      break;

    case LIBHAL_DRIVE_TYPE_DISK:
    case LIBHAL_DRIVE_TYPE_REMOVABLE_DISK:
      /* lookup an icon based on the type and the bus */
      icon_name = exo_hal_lookup_icon_name (0x10000 + type * 0x100 + bus);
      if (G_LIKELY (icon_name != NULL))
        icon_list = g_list_append (icon_list, g_strdup (icon_name));
      break;

    default:
      /* fallback below */
      break;
    }

  /* lookup an icon based solely on the drive type */
  icon_name = exo_hal_lookup_icon_name (0x10000 + type * 0x100);
  if (G_LIKELY (icon_name != NULL))
    icon_list = g_list_append (icon_list, g_strdup (icon_name));
#endif

  /* gnome-dev-removable is always the last fallback */
  return g_list_append (icon_list, g_strdup ("gnome-dev-removable"));
}



/**
 * exo_hal_volume_compute_display_name:
 * @context : a #LibHalContext, connected to the HAL daemon.
 * @volume  : a #LibHalVolume.
 * @drive   : the #LibHalDrive of the @volume.
 *
 * Similar to exo_hal_drive_compute_display_name(), but tries to find a
 * suitable display name for the @volume first, falling back to @drive
 * under certain conditions. This function may return %NULL if no
 * suitable display name was found.
 *
 * The caller is responsible to free the returned string using g_free()
 * when no longer needed.
 *
 * Return value: the display name for the @volume or %NULL if the
 *               function is unable to determine the display name.
 *
 * Since: 0.3.1.13
 **/
gchar*
exo_hal_volume_compute_display_name (struct LibHalContext_s *context,
                                     struct LibHalVolume_s  *volume,
                                     struct LibHalDrive_s   *drive)
{
#ifdef HAVE_HAL
  static const gchar UNITS[] = "KMGT";
  const gchar       *label;
  guint64            size;
  guint64            m;
  gchar             *display_name;
  gchar             *size_string;
  guint              n;


  g_return_val_if_fail (context != NULL, NULL);
  g_return_val_if_fail (volume != NULL, NULL);
  g_return_val_if_fail (drive != NULL, NULL);

  /* check if the volume specifies a usable label */
  label = libhal_volume_get_label (volume);
  if (G_LIKELY (label != NULL && *label != '\0'))
    {
      /* just use the volume label */
      display_name = g_strdup (label);
    }
  else
    {
      /* guess display name based on the drive type */
      switch (libhal_drive_get_type (drive))
        {
        case LIBHAL_DRIVE_TYPE_CDROM:
          /* check if we don't have a pure audio disc */
          if (libhal_volume_disc_has_data (volume) || !libhal_volume_disc_has_audio (volume))
            {
              /* handle (blank) data discs */
              switch (libhal_volume_get_disc_type (volume))
                {
                case LIBHAL_VOLUME_DISC_TYPE_CDR:
                  label = "CD-R";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_CDRW:
                  label = "CD-RW";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_DVDROM:
                  label = "DVD-ROM";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_DVDRAM:
                  label = "DVD-RAM";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_DVDR:
                  label = "DVD-R";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_DVDRW:
                  label = "DVD-RW";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_DVDPLUSR:
                  label = "DVD+R";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_DVDPLUSRW:
                  label = "DVD+RW";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_DVDPLUSR_DL:
                  label = "DVD+R DL";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_BDROM:
                  label = "BD-ROM";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_BDR:
                  label = "BD-R";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_BDRE:
                  label = "BD-RE";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_HDDVDROM:
                  label = "HD DVD-ROM";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_HDDVDR:
                  label = "HD DVD-R";
                  break;

                case LIBHAL_VOLUME_DISC_TYPE_HDDVDRW:
                  label = "HD DVD-RW";
                  break;

                default: /* everything else is just a CD-ROM */
                  label = "CD-ROM";
                  break;
                }

              /* display name depends on whether the disc is blank */
              if (G_UNLIKELY (libhal_volume_disc_is_blank (volume)))
                display_name = g_strdup_printf (_("Blank %s Disc"), label);
              else
                display_name = g_strdup_printf (_("%s Disc"), label);
            }
          else
            {
              /* special case for pure audio disc */
              display_name = g_strdup (_("Audio CD"));
            }
          break;

        case LIBHAL_DRIVE_TYPE_FLOPPY:
        case LIBHAL_DRIVE_TYPE_COMPACT_FLASH:
        case LIBHAL_DRIVE_TYPE_MEMORY_STICK:
        case LIBHAL_DRIVE_TYPE_SMART_MEDIA:
        case LIBHAL_DRIVE_TYPE_SD_MMC:
        case LIBHAL_DRIVE_TYPE_ZIP:
        case LIBHAL_DRIVE_TYPE_JAZ:
        case LIBHAL_DRIVE_TYPE_FLASHKEY:
        case LIBHAL_DRIVE_TYPE_PORTABLE_AUDIO_PLAYER:
          /* use the display name of the drive instead */
          display_name = exo_hal_drive_compute_display_name (context, drive);
          break;

        default:
          /* fallback to size of media */
          size = libhal_volume_get_size (volume);
          for (m = 1000, n = 0;; m *= 1000, ++n)
            {
              /* check if we found the unit */
              if (UNITS[n + 1] == '\0' || size < m * 1000u)
                {
                  /* display a comma number if result is a single digit */
                  if (G_LIKELY (size < n * 10))
                    size_string = g_strdup_printf ("%.01f%c", ((gdouble) size) / ((gdouble) m), UNITS[n]);
                  else
                    size_string = g_strdup_printf ("%lld%c", size / m, UNITS[n]);
                  break;
                }
            }

          /* generate the display name from the size string */
          if (G_UNLIKELY (libhal_drive_uses_removable_media (drive)))
            display_name = g_strdup_printf (_("%s Removable Volume"), size_string);
          else
            display_name = g_strdup_printf (_("%s Volume"), size_string);
          g_free (size_string);
        }
    }

  return display_name;
#else
  /* HAL is not available, impossible to figure out
   * a usable display name for the given volume...
   */
  return NULL;
#endif
}



/**
 * exo_hal_volume_compute_icon_list:
 * @context    : a #LibHalContext, connected to the HAL daemon.
 * @volume     : a #LibHalVolume.
 * @drive      : the #LibHalDrive of the @volume.
 *
 * Similar to exo_hal_drive_compute_icon_name(), but first looks for
 * icons for @volume, falling back to an icons for @drive.
 *
 * The caller is responsible to free the returned list using
 * <informalexample><programlisting>
 * g_list_foreach (list, (GFunc) g_free, NULL);
 * g_list_free (list);
 * </programlisting></informalexample>
 * when no longer needed.
 *
 * Return value: a list of icon names for icons that can be used to visually
 *               represent the @volume to the user.
 *
 * Since: 0.3.1.13
 **/
GList*
exo_hal_volume_compute_icon_list (struct LibHalContext_s *context,
                                  struct LibHalVolume_s  *volume,
                                  struct LibHalDrive_s   *drive)
{
  GList       *icon_list = NULL;

#ifdef HAVE_HAL
  const gchar *icon_name;

  g_return_val_if_fail (context != NULL, NULL);
  g_return_val_if_fail (volume != NULL, NULL);
  g_return_val_if_fail (drive != NULL, NULL);

  /* check if a dedicated volume icon name is known */
  icon_name = libhal_drive_get_dedicated_icon_volume (drive);
  if (G_UNLIKELY (icon_name != NULL))
    icon_list = g_list_append (icon_list, g_strdup (icon_name));

  /* check if we have a disc based volume */
  if (libhal_volume_is_disc (volume))
    {
      /* look for an icon for this specific disc type */
      icon_name = exo_hal_lookup_icon_name (HAL_ICON_DISC_CDROM + libhal_volume_get_disc_type (volume));
      if (G_LIKELY (icon_name != NULL))
        icon_list = g_list_append (icon_list, g_strdup (icon_name));
    }
#endif

  /* merge with the drive specific icon list */
  return g_list_concat (icon_list, exo_hal_drive_compute_icon_list (context, drive));
}


