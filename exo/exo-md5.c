/* $Id$ */
/*-
 * Copyright (c) 2004-2007 os-cillation e.K.
 * Copyright (c) 2004      James M. Cape <jcape@ignore-your.tv>
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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo-md5.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>
#include <exo/exo-alias.h>



/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest. The original code was
 * written by Colin Plumb in 1993, and put in the public domain.
 * 
 * Modified to use glib datatypes. Put under GPL to simplify
 * licensing for ROX-Filer. Taken from Debian's dpkg package.
 */

#define md5byte unsigned char

typedef struct _MD5_CTX MD5_CTX;

struct _MD5_CTX {
	guint32 buf[4];
	guint32 bytes[2];
	guint32 in[16];
};



#if G_BYTE_ORDER == G_BIG_ENDIAN
static void
byteSwap (guint32 *buf, unsigned words)
{
	md5byte *p = (md5byte *)buf;

	do {
		*buf++ = (guint32)((unsigned)p[3] << 8 | p[2]) << 16 |
			((unsigned)p[1] << 8 | p[0]);
		p += 4;
	} while (--words);
}
#else
#define byteSwap(buf,words)
#endif



/*
 * Start MD5 accumulation. Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void
MD5Init (MD5_CTX *ctx)
{
	ctx->buf[0] = 0x67452301;
	ctx->buf[1] = 0xefcdab89;
	ctx->buf[2] = 0x98badcfe;
	ctx->buf[3] = 0x10325476;

	ctx->bytes[0] = 0;
	ctx->bytes[1] = 0;
}



/* The four core functions - F1 is optimized somewhat */



/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s) \
	 (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)



/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void
MD5Transform (guint32 buf[4], guint32 const in[16])
{
	register guint32 a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}



/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void
MD5Update (MD5_CTX *ctx, md5byte const *buf, unsigned len)
{
	guint32 t;

	/* Update byte count */

	t = ctx->bytes[0];
	if ((ctx->bytes[0] = t + len) < t)
		ctx->bytes[1]++;	/* Carry from low to high */

	t = 64 - (t & 0x3f);	/* Space available in ctx->in (at least 1) */
	if (t > len) {
		memcpy((md5byte *)ctx->in + 64 - t, buf, len);
		return;
	}
	/* First chunk is an odd size */
	memcpy((md5byte *)ctx->in + 64 - t, buf, t);
	byteSwap(ctx->in, 16);
	MD5Transform(ctx->buf, ctx->in);
	buf += t;
	len -= t;

	/* Process data in 64-byte chunks */
	while (len >= 64) {
		memcpy(ctx->in, buf, 64);
		byteSwap(ctx->in, 16);
		MD5Transform(ctx->buf, ctx->in);
		buf += 64;
		len -= 64;
	}

	/* Handle any remaining bytes of data. */
	memcpy(ctx->in, buf, len);
}



/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 * Returns the newly allocated string of the hash.
 */
static void
MD5Final (unsigned char digest[16], MD5_CTX *ctx)
{
	int count = ctx->bytes[0] & 0x3f;	/* Number of bytes in ctx->in */
	md5byte *p = (md5byte *)ctx->in + count;

	/* Set the first char of padding to 0x80.  There is always room. */
	*p++ = 0x80;

	/* Bytes of padding needed to make 56 bytes (-8..55) */
	count = 56 - 1 - count;

	if (count < 0) {	/* Padding forces an extra block */
		memset(p, 0, count + 8);
		byteSwap(ctx->in, 16);
		MD5Transform(ctx->buf, ctx->in);
		p = (md5byte *)ctx->in;
		count = 56;
	}
	memset(p, 0, count);
	byteSwap(ctx->in, 14);

	/* Append length in bits and transform */
	ctx->in[14] = ctx->bytes[0] << 3;
	ctx->in[15] = ctx->bytes[1] << 3 | ctx->bytes[0] >> 29;
	MD5Transform (ctx->buf, ctx->in);

	byteSwap(ctx->buf, 4);

  memcpy (digest, ctx->buf, 16);
}



static void
get_md5 (const gchar *contents,
         guchar       digest[16])
{
  MD5_CTX ctx;

  MD5Init (&ctx);
  MD5Update (&ctx, (const guchar *) contents, strlen (contents));
  MD5Final (digest, &ctx);
}



GType
exo_md5_digest_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = g_boxed_type_register_static (I_("ExoMd5Digest"),
                                           (GBoxedCopyFunc) exo_md5_digest_dup,
                                           (GBoxedFreeFunc) exo_md5_digest_free);
    }

  return type;
}



/**
 * exo_str_get_md5_digest:
 * @contents : The string to create a digest of.
 *
 * Creates a binary MD5 digest of the string @contents.
 *
 * Return value: A new binary MD5 digest. It should be freed
 *               with exo_md5_digest_free() when no longer
 *               needed.
 **/
ExoMd5Digest*
exo_str_get_md5_digest (const gchar *contents)
{
  ExoMd5Digest *digest;

  g_return_val_if_fail (contents != NULL, NULL);

  digest = _exo_slice_new (ExoMd5Digest);
  get_md5 (contents, digest->digest);

  return digest;
}



/**
 * exo_str_get_md5_str:
 * @contents : The string to create a digest of.
 *
 * Creates a character array MD5 digestof the string
 * @contents.
 *
 * Return value: A newly-allocated character array which
 *               should be free with g_free() when no
 *               longer needed.
 **/
gchar*
exo_str_get_md5_str (const gchar *contents)
{
  ExoMd5Digest digest;

  g_return_val_if_fail (contents != NULL, NULL);

  get_md5 (contents, digest.digest);

  return exo_md5_digest_to_str (&digest);
}



/**
 * exo_md5_str_to_digest:
 * @str_digest : The character array digest to convert.
 *
 * Converts thq @str_digest character array digest
 * into a binary digest.
 *
 * Return value: A newly allocated binary digest. It should
 *               be freed with exo_md5_digest_free() when
 *               no longer needed.
 **/
ExoMd5Digest*
exo_md5_str_to_digest (const gchar *str_digest)
{
  ExoMd5Digest *digest;
  guint         n;

  g_return_val_if_fail (str_digest != NULL, NULL);
  g_return_val_if_fail (strlen (str_digest) == 32, NULL);

  digest = _exo_slice_new (ExoMd5Digest);
  for (n = 0; n < 16; ++n)
    {
      digest->digest[n] =
        g_ascii_xdigit_value (str_digest[2 * n]) << 4 |
        g_ascii_xdigit_value (str_digest[2 * n + 1]);
    }

  return digest;
}



/**
 * exo_md5_digest_to_str:
 * @digest : The binary MD5 digest to convert.
 *
 * Converts the binary @digest to an ASCII character array
 * digest. The result can be used as an ordinary C string.
 *
 * Return value: A newly-allocated character array which
 *               should be freed with g_free() when no
 *               longer needed.
 **/
gchar*
exo_md5_digest_to_str (const ExoMd5Digest *digest)
{
  static const gchar HEX_DIGITS[] = "0123456789abcdef";
  guchar            *str_digest;
  guint              n;

  g_return_val_if_fail (digest != NULL, NULL);

  str_digest = g_new (guchar, 33);
  for (n = 0; n < 16; n++)
    {
      str_digest[2 * n]     = HEX_DIGITS[digest->digest[n] >> 4];
      str_digest[2 * n + 1] = HEX_DIGITS[digest->digest[n] & 0xf];
    }
  str_digest[32] = 0;

  return (gchar *) str_digest;
}



/**
 * exo_md5_digest_dup:
 * @digest : The MD5 digest to copy.
 *
 * Duplicates the contents of the @digest binary
 * MD5 digest.
 *
 * Return value: A new binary MD5 digest. It should
 *               be freed with exo_md5_digest_free()
 *               when no longer needed.
 **/
ExoMd5Digest*
exo_md5_digest_dup (const ExoMd5Digest *digest)
{
  ExoMd5Digest *duplicate;

  if (G_LIKELY (digest != NULL))
    {
      /* take a copy of the digest */
      duplicate = _exo_slice_new (ExoMd5Digest);
      memcpy (duplicate, digest, sizeof (*digest));
      return duplicate;
    }
  else
    {
      /* duplicating NULL yields NULL */
      return NULL;
    }
}



/**
 * exo_md5_digest_free:
 * @digest : The MD5 digest to free.
 *
 * Frees the memory allocated for the MD5 binary
 * @digest.
 **/
void
exo_md5_digest_free (ExoMd5Digest *digest)
{
  _exo_slice_free (ExoMd5Digest, digest);
}



/**
 * exo_md5_digest_hash:
 * @digest : The #ExoMd5Digest to hash.
 *
 * Gets the numeric hash of @digest, for use
 * in #GHashTable and #GCache.
 *
 * Return value: An unsigned integer hash of
 *               the digest;
 **/
guint
exo_md5_digest_hash (gconstpointer digest)
{
  return *((guint *) digest);
}



/**
 * exo_md5_digest_equal:
 * @digest1: the first #ExoMd5Digest to compare.
 * @digest2: the second #ExoMd5Digest to compare.
 * 
 * Tests the equality of @digest1 and @digest2, useful for #GHashTable and
 * #GCashe.
 * 
 * Returns: %TRUE if both digests are equal, %FALSE otherwise.
 **/
gboolean
exo_md5_digest_equal (gconstpointer digest1,
                      gconstpointer digest2)
{
  guint *d1;
  guint *d2;
  guint  i;

  /* Both NULL or same digest */
  if (digest1 == digest2)
    return TRUE;

  /* One is NULL and the other isn't */
  if (digest1 == NULL || digest2 == NULL)
    return FALSE;

  d1 = (guint *) digest1;
  d2 = (guint *) digest2;

  for (i = 0; i < (16 / sizeof (guint)); ++i)
    {
      if (*d1 != *d2)
        return FALSE;

      d1 += i;
      d2 += i;
    }

  return TRUE;
}



#define __EXO_MD5_C__
#include <exo/exo-aliasdef.c>
