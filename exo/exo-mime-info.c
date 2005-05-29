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



static void     exo_mime_info_class_init    (ExoMimeInfoClass      *klass);
static void     exo_mime_info_init          (ExoMimeInfo           *info);
static void     exo_mime_info_finalize      (GObject               *object);
static void     exo_mime_info_get_property  (GObject               *object,
                                             guint                  prop_id,
                                             GValue                *value,
                                             GParamSpec            *pspec);
static void     exo_mime_info_set_property  (GObject               *object,
                                             guint                  prop_id,
                                             const GValue          *value,
                                             GParamSpec            *pspec);



struct _ExoMimeInfoClass
{
  GObjectClass __parent__;
};

struct _ExoMimeInfo
{
  GObject __parent__;

  gchar *comment;
  gchar *name;
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
                                                        G_PARAM_READABLE));

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
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}



static void
exo_mime_info_init (ExoMimeInfo *info)
{
  info->comment = NULL;
  info->name = NULL;
}



static void
exo_mime_info_finalize (GObject *object)
{
  ExoMimeInfo *info = EXO_MIME_INFO (object);

  g_free (info->comment);
  g_free (info->name);

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

  switch (prop_id)
    {
    case PROP_NAME:
      if (info->name == 0)
        info->name = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



/**
 * exo_mime_info_get_comment:
 * @info  : An #ExoMimeInfo instance.
 *
 * Return value: The comment associated with the @info or the empty string
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
        info->comment = g_strdup ("");
    }

  return info->comment;
}



/**
 * exo_mime_info_get_name:
 * @info  : An #ExoMimeInfo instance.
 *
 * Returns the full qualified name of the MIME type described
 * by the @info object.
 *
 * Return value: The name of @info.
 */
const gchar*
exo_mime_info_get_name (ExoMimeInfo *info)
{
  g_return_val_if_fail (EXO_IS_MIME_INFO (info), NULL);
  return info->name;
}




