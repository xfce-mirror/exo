/* $Id$ */
/*-
 * Copyright (c) 2004 os-cillation e.K.
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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo.h>



static gchar  *decode (const gchar *string,
                       gint         length);

static void exo_uri_finalize (GObject *object);



struct _ExoUriClass
{
  GObjectClass  __parent__;
};

struct _ExoUri
{
  GObject     __parent__;
  gchar      *scheme;
  gchar      *host;
  gchar      *path;
  gchar       md5sum[33];
};



static gchar         file_scheme[]    = "file";
static gchar         file_localhost[] = "localhost";
static GObjectClass *parent_class;



static gchar*
decode (const gchar *string,
        gint         length)
{
  const gchar *send;
  const gchar *sp;
  gchar       *tp;
  gchar       *rv;
  guint        n;

  if (length < 0)
    length = strlen (string);
  send = string + length;

  rv = g_malloc (length + 1);
  for (sp = string, tp = rv; sp < send; )
    {
      if (G_UNLIKELY (sp[0] == '%'
                   && g_ascii_isxdigit (sp[1])
                   && g_ascii_isxdigit (sp[2])))
        {
          if (sp[1] >= '0' && sp[1] <= '9')
            n = sp[1] - '0';
          else if (sp[1] >= 'A' && sp[1] <= 'F')
            n = sp[1] - ('A' - 10);
          else
            n = sp[1] - ('a' - 10);

          n *= 16;

          if (sp[2] >= '0' && sp[2] <= '9')
            n += sp[2] - '0';
          else if (sp[2] >= 'A' && sp[2] <= 'F')
            n += sp[2] - ('A' - 10);
          else
            n += sp[2] - ('a' - 10);

          *((guchar *) tp) = n;

          tp += 1;
          sp += 3;
        }
      else
        {
          *tp++ = *sp++;
        }
    }
  *tp = '\0';

  /* strip any trailing slash */
  if (G_UNLIKELY (tp - 1 > rv && *(tp - 1) == '/'))
    *(tp - 1) = '\0';

  return rv;
}



G_DEFINE_TYPE (ExoUri, exo_uri, G_TYPE_OBJECT);



static void
exo_uri_class_init (ExoUriClass *klass)
{
  GObjectClass *gobject_class;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize = exo_uri_finalize;
}



static void
exo_uri_init (ExoUri *uri)
{
}



static void
exo_uri_finalize (GObject *object)
{
  ExoUri *uri = (ExoUri *) object;

  if (G_LIKELY (uri->scheme != NULL) && uri->scheme != file_scheme)
    g_free (uri->scheme);
  if (G_LIKELY (uri->host != NULL) && uri->host != file_localhost)
    g_free (uri->host);
  if (G_LIKELY (uri->path != NULL))
    g_free (uri->path);

  parent_class->finalize (object);
}



GQuark
exo_uri_error_quark (void)
{
  static GQuark quark = 0;

  if (G_UNLIKELY (quark == 0))
    {
      quark = g_quark_from_static_string ("xfce-uri-error-quark");
    }

  return quark;
}



/**
 * exo_uri_new:
 * @url         :
 * @error       :
 *
 * Return value : %NULL on error
 **/
ExoUri*
exo_uri_new (const gchar *identifier,
              GError     **error)
{
  const gchar *p = identifier;
  const gchar *t;
  ExoUri     *uri;

  g_return_val_if_fail (identifier != NULL && *identifier != '\0', NULL);

  uri = g_object_new (EXO_TYPE_URI, NULL);
  uri->scheme = file_scheme;

  if (p[0] == '/')
    {
legacy:
      uri->host   = file_localhost;
      uri->path   = decode (p, -1);
    }
  else if (p[0] == 'f' && p[1] == 'i' && p[2] == 'l'
        && p[3] == 'e' && p[4] == ':' && p[5] != '\0')
    {
      if (p[5] == '/' && p[6] != '/')
        {
          p += 5;
          goto legacy;
        }
      else
        {
          p += 7;
        }

      if (*p == '/')
        {
          /* file:///<path> */
          uri->host = file_localhost;
        }
      else
        {
          /* file://<host>/<path> */
          for (t = p, ++p; *p != '/'; ++p)
            if (G_UNLIKELY (*p == '\0'))
              goto error;

          uri->host = g_new (gchar, (p - t) + 1);
          strncpy (uri->host, t, p - t);
          uri->host[p - t] = '\0';
        }

      uri->path = decode (p, -1);
    }
  else
    goto error;

  return uri;

error:
  g_set_error (error, EXO_URI_ERROR, EXO_URI_ERROR_INVALID,
               _("Invalid URI '%s'"), identifier);
  g_object_unref (uri);
  return NULL;
}



/**
 * exo_uri_parent:
 * @uri         :
 *
 * Return value :
 **/
ExoUri*
exo_uri_parent (const ExoUri *uri)
{
  ExoUri *parent;

  g_return_val_if_fail (EXO_IS_URI (uri), NULL);

  if (uri->path[0] == '/' && uri->path[1] == '\0')
    return NULL;

  parent = g_object_new (EXO_TYPE_URI, NULL);

  if (uri->scheme == file_scheme)
    parent->scheme = file_scheme;
  else if (uri->scheme != NULL)
    parent->scheme = g_strdup (uri->scheme);

  if (uri->host == file_localhost)
    parent->host = file_localhost;
  else if (uri->host != NULL)
    parent->host = g_strdup (uri->host);

  parent->path = g_path_get_dirname (uri->path);

  return parent;
}



/**
 * exo_uri_relative:
 * @uri         :
 * @name        :
 *
 * Return value :
 **/
ExoUri*
exo_uri_relative (const ExoUri *uri,
                   const gchar   *name)
{
  ExoUri *relative;

  g_return_val_if_fail (EXO_IS_URI (uri), NULL);

  relative = g_object_new (EXO_TYPE_URI, NULL);

  if (uri->scheme == file_scheme)
    relative->scheme = file_scheme;
  else if (uri->scheme != NULL)
    relative->scheme = g_strdup (uri->scheme);

  if (uri->host == file_localhost)
    relative->host = file_localhost;
  else if (uri->host != NULL)
    relative->host = g_strdup (uri->host);

  relative->path = g_build_filename (uri->path, name, NULL);

  return relative;
}



/**
 * exo_uri_get_scheme:
 * @uri         : a valid #ExoUri object.
 *
 * Returns the scheme of the URI, e.g. 'file' for file URIs.
 *
 * Return value : the scheme of @uri.
 **/
const gchar*
exo_uri_get_scheme (const ExoUri *uri)
{
  g_return_val_if_fail (EXO_IS_URI (uri), NULL);
  return uri->scheme;
}



/**
 * exo_uri_get_host:
 * @uri         : a valid #ExoUri object.
 *
 * Returns the hostname of the URI. The hostname returned won't be
 * escaped in any way, that is non-ascii characters won't be encoded.
 *
 * There is a special case for file URIs to take into account here:
 * For file URIs in the form 'file:///path' and 'file:/path' the hostname
 * returned here will be 'localhost'.
 *
 * Return value : the hostname of @uri.
 **/
const gchar*
exo_uri_get_host (const ExoUri *uri)
{
  g_return_val_if_fail (EXO_IS_URI (uri), NULL);
  return uri->host;
}



/**
 * exo_uri_get_path:
 * @uri         : a valid #ExoUri object.
 *
 * Returns the path component of the URI. The path returned won't be
 * escaped in any way, that says non-ascii characters won't be encoded.
 *
 * Return value : the path of @uri.
 **/
const gchar*
exo_uri_get_path (const ExoUri *uri)
{
  g_return_val_if_fail (EXO_IS_URI (uri), NULL);
  return uri->path;
}



/**
 * exo_uri_get_md5sum:
 * @uri         :
 *
 * XXX - Remove me?
 *
 * Return value :
 **/
const gchar*
exo_uri_get_md5sum (const ExoUri *uri)
{
  g_return_val_if_fail (EXO_IS_URI (uri), NULL);

  if (G_UNLIKELY (uri->md5sum[0] == '\0'))
    {
    }

  return uri->md5sum;
}



/**
 * exo_uri_is_local:
 * @uri         : a valid #ExoUri object.
 *
 * Checks if the given URI references a local entity. An entity is considered
 * local if it is scheme is 'file'.
 *
 * FIXME: Check NFS/AFS file systems here? No.
 *
 * Return value : %TRUE if @uri is considered local, else %FALSE.
 **/
gboolean
exo_uri_is_local (const ExoUri *uri)
{
  const gchar *s;

  g_return_val_if_fail (EXO_IS_URI (uri), FALSE);

  s = uri->scheme;
  return s[0] == 'f' && s[1] == 'i'
      && s[2] == 'l' && s[3] == 'e'
      && s[4] == '\0';
}



/**
 * exo_uri_is_root:
 * @uri         :
 *
 * Return value :
 **/
gboolean
exo_uri_is_root (const ExoUri *uri)
{
#ifdef EXO_DEBUG_PARAMETERS
  g_assert (EXO_IS_URI (uri));
#endif

  return uri->path[0] == '/' && uri->path[1] == '\0';
}



/**
 * exo_uri_encode:
 * @uri         : a valid #ExoUri object.
 * @flags       : for now, only %EXO_URI_ENCODE_WITH_HOST
 *
 * Returns the URI as encoded/escaped string.
 *
 * Return value : @uri with all non-ascii characters encoded/escaped.
 *                Value should be freed when no longer needed.
 **/
gchar*
exo_uri_encode (const ExoUri *uri,
                 guint          flags)
{
  static const guchar hex[16] = "0123456789ABCDEF";
  const guchar       *sp;
  guchar             *tend;
  guchar             *tp;
  guchar              buffer[4096];
  guint               n;

  g_return_val_if_fail (EXO_IS_URI (uri), NULL);

  /* prepend scheme */
  n = g_strlcpy (buffer, uri->scheme, 4096);
  buffer[n++] = ':'; buffer[n++] = '/'; buffer[n++] = '/';

  tp = buffer + n;
  tend = buffer + (4096 - 1);

  if ((flags & EXO_URI_ENCODE_WITH_HOST) && G_LIKELY (uri->host != NULL))
    {
      for (sp = uri->host; *sp != '\0' && tp < tend; )
        *tp++ = *sp++;
    }

  for (sp = uri->path; *sp != '\0' && tp < tend; ++sp)
    {
      if (G_UNLIKELY (*sp > 0x7fu))
        {
          *tp++ = '%';
          *tp++ = hex[*sp >> 4];
          *tp++ = hex[*sp & 15];
        }
      else
        {
          *tp++ = *sp;
        }
    }

  *tp = '\0';

  return g_strdup ((const gchar *) buffer);
}



/**
 * exo_uri_to_utf8:
 * @uri         :
 * @flags       :
 *
 * Return value :
 **/
gchar*
exo_uri_to_utf8 (const ExoUri *uri,
                  guint          flags)
{
  gchar *filename;
  gchar *result;

  g_return_val_if_fail (EXO_IS_URI (uri), NULL);

  filename = g_filename_to_utf8 (uri->path, -1, NULL, NULL, NULL);
  if (flags & EXO_URI_ENCODE_WITH_HOST)
    result = g_strconcat ("file://", uri->host, filename, NULL);
  else
    result = g_strconcat ("file://", filename, NULL);
  g_free (filename);

  return result;
}



/**
 * exo_uri_hash:
 * @uri         :
 *
 * Return value :
 **/
guint
exo_uri_hash (const ExoUri  *uri)
{
  const guchar *p;
  guint         h = 0;

  g_return_val_if_fail (EXO_IS_URI (uri), 0);

  if (G_LIKELY (uri->scheme != NULL))
    for (p = uri->scheme; *p != '\0'; ++p)
      h = (h << 5) - h + *p;

  if (G_LIKELY (uri->host != NULL))
    for (p = uri->host; *p != '\0'; ++p)
      h = (h << 5) - h + *p;

  if (G_LIKELY (uri->path != NULL))
    for (p = uri->path; *p != '\0'; ++p)
      h = (h << 5) - h + *p;

  return h;
}



/**
 * exo_uri_equal:
 * @a           :
 * @b           :
 *
 * Return value :
 **/
gboolean
exo_uri_equal (const ExoUri *a,
               const ExoUri *b)
{
  g_return_val_if_fail (EXO_IS_URI (a), FALSE);
  g_return_val_if_fail (EXO_IS_URI (b), FALSE);

  /* FIXME! */
  return (strcmp (a->path, b->path) == 0);
}


