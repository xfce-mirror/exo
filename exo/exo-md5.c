/* $Id: exo-md5.c,v 1.1.1.1 2004/09/14 22:32:58 bmeurer Exp $ */
/*-
 * Copyright (c) 2004  Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_MD5_H
#include <md5.h>
#else
#ifdef HAVE_OPENSSL_MD5_H
#include <openssl/md5.h>
#endif
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo-md5.h>



#if !defined(HAVE_MD5INIT) && !defined(HAVE_MD5_INIT)
#error "System lacks MD5Init and MD5_Init!"
#endif



#ifndef HAVE_MD5INIT
#define MD5Init(ctx)              MD5_Init((ctx))
#define MD5Update(ctx, buf, len)  MD5_Update((ctx), (buf), (len))

static char*
MD5End (MD5_CTX *ctx, char *buf)
{
  int i;
  unsigned char digest[16];
  static const char hex[] = "0123456789abcdef";

  MD5_Final (digest, ctx);

  for (i = 0; i < 16; i++)
    {
      buf[i+i] = hex[digest[i] >> 4];
      buf[i+i+1] = hex[digest[i] & 0x0f];
    }
  buf[i+i] = '\0';

  return buf;
}

#endif



/**
 * exo_md5_calculate_hash:
 * @source      :
 * @buffer      :
 * @length      :
 *
 * Return value :
 **/
gchar*
exo_md5_calculate_hash (const gchar *source,
                         gchar       *buffer,
                         gsize        length)
{
  MD5_CTX context;

  g_return_val_if_fail (source != NULL, NULL);
  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (length >= 33, NULL);

  MD5Init (&context);
  MD5Update (&context, source, strlen (source));
  return MD5End (&context, buffer);
}


