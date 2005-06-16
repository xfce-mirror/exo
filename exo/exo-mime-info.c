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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo/exo-config.h>
#include <exo/exo-mime-info.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>

#include <xdgmime/xdgmime.h>



enum
{
  PROP_0,
  PROP_COMMENT,
  PROP_NAME,
};

static const struct
{
  const gchar *const type;
  const gchar *const icon;
} GNOME_ICONNAMES[] =
{
  { "inode/directory", "gnome-fs-directory" },
  { "inode/blockdevice", "gnome-fs-blockdev" },
  { "inode/chardevice", "gnome-fs-chardev" },
  { "inode/fifo", "gnome-fs-fifo" },
  { "inode/socket", "gnome-fs-socket" },
};



static void     exo_mime_info_class_init          (ExoMimeInfoClass *klass);
static void     exo_mime_info_init                (ExoMimeInfo      *info);
static void     exo_mime_info_finalize            (GObject          *object);
static void     exo_mime_info_get_property        (GObject          *object,
                                                   guint             prop_id,
                                                   GValue           *value,
                                                   GParamSpec       *pspec);
static void     exo_mime_info_set_property        (GObject          *object,
                                                   guint             prop_id,
                                                   const GValue     *value,
                                                   GParamSpec       *pspec);
static void     exo_mime_info_icon_theme_changed  (GtkIconTheme     *icon_theme,
                                                   ExoMimeInfo      *info);



struct _ExoMimeInfoClass
{
  GObjectClass __parent__;
};

struct _ExoMimeInfo
{
  GObject __parent__;

  gchar        *comment;
  gchar        *name;

  const gchar  *media;
  const gchar  *subtype;

  gchar        *icon_name;
  gboolean      icon_name_static : 1;
  GtkIconTheme *icon_theme;
};



static GObjectClass *parent_class;



G_DEFINE_TYPE (ExoMimeInfo, exo_mime_info, G_TYPE_OBJECT);



static void
exo_mime_info_class_init (ExoMimeInfoClass *klass)
{
  GObjectClass *gobject_class;

  /* initialize the libexo i18n support */
  _exo_i18n_init ();

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_mime_info_finalize;
  gobject_class->get_property = exo_mime_info_get_property;
  gobject_class->set_property = exo_mime_info_set_property;

  /**
   * ExoMimeInfo:comment:
   *
   * A human readable text describing the MIME type. This can be
   * the empty string if no comment was provided for the given
   * MIME type.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_COMMENT,
                                   g_param_spec_string ("comment",
                                                        _("Mime info comment"),
                                                        _("The comment gives a better description of the MIME type"),
                                                        NULL,
                                                        EXO_PARAM_READABLE));

  /**
   * ExoMimeInfo:name:
   *
   * The name of the MIME type described by this #ExoMimeInfo
   * instance. The name is always the full qualified MIME
   * type, e.g. "application/octect-stream" or "text/plain".
   */
  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        _("Mime info name"),
                                                        _("The full qualified name of the mime type"),
                                                        NULL,
                                                        EXO_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}



static void
exo_mime_info_init (ExoMimeInfo *info)
{
  /* nothing to do here, as GObject already takes care of clearing
   * the memory and thereby setting all pointers to NULL.
   */
}



static void
exo_mime_info_finalize (GObject *object)
{
  ExoMimeInfo *info = EXO_MIME_INFO (object);

  /* release comment/name info */
  if (info->comment != NULL && info->comment != info->name)
    g_free (info->comment);
  g_free (info->name);

  /* disconnect from the icon theme (if any) */
  if (G_LIKELY (info->icon_theme != NULL))
    {
      g_signal_handlers_disconnect_by_func (G_OBJECT (info->icon_theme), exo_mime_info_icon_theme_changed, info);
      g_object_unref (G_OBJECT (info->icon_theme));
    }

  /* free the icon name if it isn't one of the statics */
  if (G_LIKELY (!info->icon_name_static && info->icon_name != NULL))
    g_free (info->icon_name);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}



static void
exo_mime_info_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  ExoMimeInfo *info = EXO_MIME_INFO (object);

  switch (prop_id)
    {
    case PROP_COMMENT:
      g_value_set_string (value, exo_mime_info_get_comment (info));
      break;

    case PROP_NAME:
      g_value_set_string (value, exo_mime_info_get_name (info));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_mime_info_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  ExoMimeInfo *info = EXO_MIME_INFO (object);
  const gchar *name;
  const gchar *s;
  const gchar *t;
  const gchar *u;
  gchar       *v;

  switch (prop_id)
    {
    case PROP_NAME:
      if (info->name == 0)
        {
          name = g_value_get_string (value);
          for (s = NULL, t = name; *t != '\0'; ++t)
            if (G_UNLIKELY (*t == '/'))
              s = t;

          /* since the property can only be set on construction
           * by the ExoMimeDatabase class, we can safely ignore
           * problems here.
           */
          if (G_UNLIKELY (s == NULL))
            return;

          /* allocate memory to store both the full name, as
           * well as the media type alone.
           */
          info->name = g_new (gchar, (t - name) + (s - name) + 2);

          /* copy the full name (including the terminator) */
          for (u = name, v = info->name; u <= t; ++u, ++v)
            *v = *u;

          /* set the subtype pointer */
          info->subtype = info->name + (s - name) + 1;

          /* copy the media portion */
          info->media = v;
          for (u = name; u < s; ++u, ++v)
            *v = *u;

          /* terminate the media portion */
          *v = '\0';
        }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_mime_info_icon_theme_changed (GtkIconTheme *icon_theme,
                                  ExoMimeInfo  *info)
{
  g_return_if_fail (GTK_IS_ICON_THEME (icon_theme));
  g_return_if_fail (EXO_IS_MIME_INFO (info));
  g_return_if_fail (info->icon_theme == icon_theme);

  /* drop the cached icon name, so the next lookup call
   * will perform a lookup again.
   */
  if (G_LIKELY (!info->icon_name_static))
    {
      g_free (info->icon_name);
      info->icon_name = NULL;
    }
}



/**
 * exo_mime_info_get_comment:
 * @info  : an #ExoMimeInfo instance.
 *
 * Return value: the comment associated with the @info or the empty string
 *               if no comment was provided.
 */
const gchar*
exo_mime_info_get_comment (ExoMimeInfo *info)
{
  gchar *path;
  gchar *spec;

  g_return_val_if_fail (EXO_IS_MIME_INFO (info), NULL);

  if (G_UNLIKELY (info->comment == NULL))
    {
      spec = g_strdup_printf ("mime/%s.xml", info->name);
      path = xfce_resource_lookup (XFCE_RESOURCE_DATA, spec);
      g_free (spec);

      if (G_LIKELY (path != NULL))
        {
          info->comment = _exo_load_mime_comment_from_file (path, NULL);
          g_free (path);
        }

      if (G_UNLIKELY (info->comment == NULL))
        info->comment = info->name;
    }

  return info->comment;
}



/**
 * exo_mime_info_get_name:
 * @info  : an #ExoMimeInfo instance.
 *
 * Returns the full qualified name of the MIME type described
 * by the @info object.
 *
 * Return value: the name of @info.
 */
const gchar*
exo_mime_info_get_name (ExoMimeInfo *info)
{
  g_return_val_if_fail (EXO_IS_MIME_INFO (info), NULL);
  return info->name;
}



/**
 * exo_mime_info_get_media:
 * @info  : an #ExoMimeInfo instance.
 *
 * Returns the media portion of the MIME type, e.g. if your #ExoMimeInfo
 * instance refers to "text/plain", invoking this method will return
 * "text".
 *
 * Return value: the media portion of the MIME type.
 */
const gchar*
exo_mime_info_get_media (ExoMimeInfo *info)
{
  g_return_val_if_fail (EXO_IS_MIME_INFO (info), NULL);
  return info->media;
}



/**
 * exo_mime_info_get_subtype:
 * @info  : an #ExoMimeInfo instance.
 *
 * Returns the subtype portion of the MIME type, e.g. if your #ExoMimeInfo
 * instance refers to "application/octect-stream", invoking this method
 * will return "octect-stream".
 *
 * Return value: the subtype portion of the MIME type.
 */
const gchar*
exo_mime_info_get_subtype (ExoMimeInfo *info)
{
  g_return_val_if_fail (EXO_IS_MIME_INFO (info), NULL);
  return info->subtype;
}



/**
 * exo_mime_info_lookup_icon_name:
 * @info       : an #ExoMimeInfo instance.
 * @icon_theme : a #GtkIconTheme instance.
 *
 * Tries to determine the name of a suitable icon for @info
 * in @icon_theme. The returned icon name can then be used
 * in calls to #gtk_icon_theme_lookup_icon() or
 * #gtk_icon_theme_load_icon().
 *
 * Return value: a suitable icon name for @info in @icon_theme.
 **/
const gchar*
exo_mime_info_lookup_icon_name (ExoMimeInfo  *info,
                                GtkIconTheme *icon_theme)
{
  gsize n;

  g_return_val_if_fail (EXO_IS_MIME_INFO (info), NULL);
  g_return_val_if_fail (GTK_IS_ICON_THEME (icon_theme), NULL);

  /* check if our cached name will suffice */
  if (G_LIKELY (info->icon_theme == icon_theme && info->icon_name != NULL))
    return info->icon_name;

  /* if we have a new icon theme, connect to the new one */
  if (G_UNLIKELY (info->icon_theme != icon_theme))
    {
      /* disconnect from the previous one */
      if (G_LIKELY (info->icon_theme != NULL))
        {
          g_signal_handlers_disconnect_by_func (G_OBJECT (info->icon_theme), exo_mime_info_icon_theme_changed, info);
          g_object_unref (G_OBJECT (info->icon_theme));
        }

      /* connect to the new one */
      info->icon_theme = icon_theme;
      g_object_ref (G_OBJECT (icon_theme));
      g_signal_connect (G_OBJECT (icon_theme), "changed", G_CALLBACK (exo_mime_info_icon_theme_changed), info);
    }

  /* free the previously set icon name */
  if (!info->icon_name_static)
    g_free (info->icon_name);

  /* start out with the full name (assuming a non-static icon_name) */
  info->icon_name = g_strdup_printf ("gnome-mime-%s-%s", info->media, info->subtype);
  info->icon_name_static = FALSE;
  if (!gtk_icon_theme_has_icon (icon_theme, info->icon_name))
    {
      /* only the media portion */
      info->icon_name[11 + ((info->subtype - 1) - info->name)] = '\0';
      if (!gtk_icon_theme_has_icon (icon_theme, info->icon_name))
        {
          /* if we get here, we'll use a static icon name */
          info->icon_name_static = TRUE;
          g_free (info->icon_name);

          /* GNOME uses non-standard names for special MIME types */
          for (n = 0; n < G_N_ELEMENTS (GNOME_ICONNAMES); ++n)
            if (exo_str_is_equal (info->name, GNOME_ICONNAMES[n].type))
              if (gtk_icon_theme_has_icon (icon_theme, GNOME_ICONNAMES[n].icon))
                {
                  info->icon_name = (gchar *)GNOME_ICONNAMES[n].icon;
                  break;
                }

          /* fallback is always application/octect-stream */
          if (n == G_N_ELEMENTS (GNOME_ICONNAMES))
            info->icon_name = (gchar *)"gnome-mime-application-octect-stream";
        }
    }

  g_assert (info->icon_name != NULL);
  g_assert (info->icon_name[0] != '\0');

  return info->icon_name;
}



/**
 * exo_mime_info_lookup_icon:
 * @info        : an #ExoMimeInfo instance.
 * @icon_theme  : reference to a #GtkIconTheme, on which the icon lookup
 *                should be performed.
 * @size        : desired icon size.
 * @flags       : flags modifying the behavior of the icon lookup.
 *
 * Tries to lookup an icon for the given MIME @info object on the given
 * @icon_theme object. The implementation will try various popular
 * icon naming styles.
 *
 * This method is basicly a wrapper for #exo_mime_info_lookup_icon_name()
 * and #gtk_icon_theme_lookup_icon(). If you don't plan to perform any
 * caching on icons, this method is what you want.
 *
 * Return value: a #GtkIconInfo structure containing information about the
 *               icon, or %NULL if the icon wasn't found. Free with the
 *               returned structure with #gtk_icon_info_free().
 */
GtkIconInfo*
exo_mime_info_lookup_icon (ExoMimeInfo        *info,
                           GtkIconTheme       *icon_theme,
                           gint                size,
                           GtkIconLookupFlags  flags)
{
  const gchar *icon_name;

  g_return_val_if_fail (EXO_IS_MIME_INFO (info), NULL);
  g_return_val_if_fail (GTK_IS_ICON_THEME (icon_theme), NULL);

  icon_name = exo_mime_info_lookup_icon_name (info, icon_theme);
  return gtk_icon_theme_lookup_icon (icon_theme, icon_name, size, flags);
}


