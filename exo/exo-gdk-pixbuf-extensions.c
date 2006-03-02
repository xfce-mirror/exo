/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
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

#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_MMINTRIN_H
#include <mmintrin.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <exo/exo-gdk-pixbuf-extensions.h>
#include <exo/exo-alias.h>



/**
 * exo_gdk_pixbuf_colorize:
 * @src   : the source #GdkPixbuf.
 * @color : the new color.
 *
 * Creates a new #GdkPixbuf based on @src, which is
 * colorized to @color.
 *
 * The caller is responsible to free the returned object
 * using g_object_unref() when no longer needed.
 *
 * Return value: the colorized #GdkPixbuf.
 **/
GdkPixbuf*
exo_gdk_pixbuf_colorize (const GdkPixbuf *src,
                         const GdkColor  *color)
{
  GdkPixbuf *dst;
  gboolean   has_alpha;
  gint       dst_row_stride;
  gint       src_row_stride;
  gint       width;
  gint       height;
  gint       i;

  /* determine source parameters */
  width = gdk_pixbuf_get_width (src);
  height = gdk_pixbuf_get_height (src);
  has_alpha = gdk_pixbuf_get_has_alpha (src);

  /* allocate the destination pixbuf */
  dst = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (src), has_alpha, gdk_pixbuf_get_bits_per_sample (src), width, height);

  /* determine row strides on src/dst */
  dst_row_stride = gdk_pixbuf_get_rowstride (dst);
  src_row_stride = gdk_pixbuf_get_rowstride (src);

#if defined(__GNUC__) && defined(__MMX__)
  /* check if there's a good reason to use MMX */
  if (G_LIKELY (has_alpha && dst_row_stride == width * 4 && src_row_stride == width * 4 && (width * height) % 2 == 0))
    {
      __m64 *pixdst = (__m64 *) gdk_pixbuf_get_pixels (dst);
      __m64 *pixsrc = (__m64 *) gdk_pixbuf_get_pixels (src);
      __m64  alpha_mask = _mm_set_pi8 (0xff, 0, 0, 0, 0xff, 0, 0, 0);
      __m64  color_factor = _mm_set_pi16 (0, color->blue, color->green, color->red);
      __m64  zero = _mm_setzero_si64 ();
      __m64  src, alpha, hi, lo;

      /* divide color components by 256 */
      color_factor = _mm_srli_pi16 (color_factor, 8);

      for (i = (width * height) >> 1; i > 0; --i)
        {
          /* read the source pixel */
          src = *pixsrc;

          /* remember the two alpha values */
          alpha = _mm_and_si64 (alpha_mask, src);

          /* extract the hi pixel */
          hi = _mm_unpackhi_pi8 (src, zero);
          hi = _mm_mullo_pi16 (hi, color_factor);

          /* extract the lo pixel */
          lo = _mm_unpacklo_pi8 (src, zero);
          lo = _mm_mullo_pi16 (lo, color_factor);

          /* prefetch the next two pixels */
          __builtin_prefetch (++pixsrc, 0, 1);

          /* divide by 256 */
          hi = _mm_srli_pi16 (hi, 8);
          lo = _mm_srli_pi16 (lo, 8);

          /* combine the 2 pixels again */
          src = _mm_packs_pu16 (lo, hi);

          /* write back the calculated color together with the alpha */
          *pixdst = _mm_or_si64 (alpha, src);

          /* advance the dest pointer */
          ++pixdst;
        }

      _mm_empty ();
    }
  else
#endif
    {
      guchar *dst_pixels = gdk_pixbuf_get_pixels (dst);
      guchar *src_pixels = gdk_pixbuf_get_pixels (src);
      guchar *pixdst;
      guchar *pixsrc;
      gint    red_value = color->red / 255.0;
      gint    green_value = color->green / 255.0;
      gint    blue_value = color->blue / 255.0;
      gint    j;

      for (i = height; --i >= 0; )
        {
          pixdst = dst_pixels + i * dst_row_stride;
          pixsrc = src_pixels + i * src_row_stride;

          for (j = width; j > 0; --j)
            {
              *pixdst++ = (*pixsrc++ * red_value) >> 8;
              *pixdst++ = (*pixsrc++ * green_value) >> 8;
              *pixdst++ = (*pixsrc++ * blue_value) >> 8;
              
              if (has_alpha)
                *pixdst++ = *pixsrc++;
            }
        }
    }

  return dst;
}



/**
 * exo_gdk_pixbuf_lucent:
 * @src     : the source #GdkPixbuf.
 * @percent : the percentage of translucency.
 *
 * Returns a version of @src, whose pixels translucency is
 * @percent of the original @src pixels.
 *
 * The caller is responsible to free the returned object
 * using g_object_unref() when no longer needed.
 *
 * Return value: a translucent version of @src.
 **/
GdkPixbuf*
exo_gdk_pixbuf_lucent (const GdkPixbuf *src,
                       guint            percent)
{
  GdkPixbuf *dst;
  guchar    *dst_pixels;
  guchar    *src_pixels;
  guchar    *pixdst;
  guchar    *pixsrc;
  gint       dst_row_stride;
  gint       src_row_stride;
  gint       width;
  gint       height;
  gint       i, j;

  g_return_val_if_fail (GDK_IS_PIXBUF (src), NULL);
  g_return_val_if_fail (percent >= 0 && percent <= 100, NULL);

  /* determine source parameters */
  width = gdk_pixbuf_get_width (src);
  height = gdk_pixbuf_get_height (src);

  /* allocate the destination pixbuf */
  dst = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (src), TRUE, gdk_pixbuf_get_bits_per_sample (src), width, height);

  /* determine row strides on src/dst */
  dst_row_stride = gdk_pixbuf_get_rowstride (dst);
  src_row_stride = gdk_pixbuf_get_rowstride (src);

  /* determine pixels on src/dst */
  dst_pixels = gdk_pixbuf_get_pixels (dst);
  src_pixels = gdk_pixbuf_get_pixels (src);

  /* check if the source already contains an alpha channel */
  if (G_LIKELY (gdk_pixbuf_get_has_alpha (src)))
    {
      for (i = height; --i >= 0; )
        {
          pixdst = dst_pixels + i * dst_row_stride;
          pixsrc = src_pixels + i * src_row_stride;

          for (j = width; --j >= 0; )
            {
              *pixdst++ = *pixsrc++;
              *pixdst++ = *pixsrc++;
              *pixdst++ = *pixsrc++;
              *pixdst++ = ((guint) *pixsrc++ * percent) / 100u;
            }
        }
    }
  else
    {
      /* pre-calculate the alpha value */
      percent = (255u * percent) / 100u;

      for (i = height; --i >= 0; )
        {
          pixdst = dst_pixels + i * dst_row_stride;
          pixsrc = src_pixels + i * src_row_stride;

          for (j = width; --j >= 0; )
            {
              *pixdst++ = *pixsrc++;
              *pixdst++ = *pixsrc++;
              *pixdst++ = *pixsrc++;
              *pixdst++ = percent;
            }
        }
    }

  return dst;
}



static inline guchar
lighten_channel (guchar cur_value)
{
  gint new_value = cur_value;

  new_value += 24 + (new_value >> 3);
  if (G_UNLIKELY (new_value > 255))
    new_value = 255;

  return (guchar) new_value;
}



/**
 * exo_gdk_pixbuf_spotlight:
 * @src : the source #GdkPixbuf.
 *
 * Creates a lightened version of @src, suitable for
 * prelit state display of icons.
 *
 * The caller is responsible to free the returned
 * pixbuf using #g_object_unref().
 *
 * Return value: the lightened version of @src.
 **/
GdkPixbuf*
exo_gdk_pixbuf_spotlight (const GdkPixbuf *src)
{
  GdkPixbuf *dst;
  gboolean   has_alpha;
  gint       dst_row_stride;
  gint       src_row_stride;
  gint       width;
  gint       height;
  gint       i;

  /* determine source parameters */
  width = gdk_pixbuf_get_width (src);
  height = gdk_pixbuf_get_height (src);
  has_alpha = gdk_pixbuf_get_has_alpha (src);

  /* allocate the destination pixbuf */
  dst = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (src), has_alpha, gdk_pixbuf_get_bits_per_sample (src), width, height);

  /* determine src/dst row strides */
  dst_row_stride = gdk_pixbuf_get_rowstride (dst);
  src_row_stride = gdk_pixbuf_get_rowstride (src);
  
#if defined(__GNUC__) && defined(__MMX__)
  /* check if there's a good reason to use MMX */
  if (G_LIKELY (has_alpha && dst_row_stride == width * 4 && src_row_stride == width * 4 && (width * height) % 2 == 0))
    {
      __m64 *pixdst = (__m64 *) gdk_pixbuf_get_pixels (dst);
      __m64 *pixsrc = (__m64 *) gdk_pixbuf_get_pixels (src);
      __m64  alpha_mask = _mm_set_pi8 (0xff, 0, 0, 0, 0xff, 0, 0, 0);
      __m64  twentyfour = _mm_set_pi8 (0, 24, 24, 24, 0, 24, 24, 24);
      __m64  zero = _mm_setzero_si64 ();

      for (i = (width * height) >> 1; i > 0; --i)
        {
          /* read the source pixel */
          __m64 src = *pixsrc;

          /* remember the two alpha values */
          __m64 alpha = _mm_and_si64 (alpha_mask, src);

          /* extract the hi pixel */
          __m64 hi = _mm_unpackhi_pi8 (src, zero);

          /* extract the lo pixel */
          __m64 lo = _mm_unpacklo_pi8 (src, zero);

          /* add (x >> 3) to x */
          hi = _mm_adds_pu16 (hi, _mm_srli_pi16 (hi, 3));
          lo = _mm_adds_pu16 (lo, _mm_srli_pi16 (lo, 3));

          /* prefetch next value */
          __builtin_prefetch (++pixsrc, 0, 1);

          /* combine the two pixels again */
          src = _mm_packs_pu16 (lo, hi);

          /* add 24 (with saturation) */
          src = _mm_adds_pu8 (src, twentyfour);

          /* drop the alpha channel from the temp color */
          src = _mm_andnot_si64 (alpha_mask, src);

          /* write back the calculated color */
          *pixdst = _mm_or_si64 (alpha, src);

          /* advance the dest pointer */
          ++pixdst;
        }

      _mm_empty ();
    }
  else
#endif
    {
      guchar *dst_pixels = gdk_pixbuf_get_pixels (dst);
      guchar *src_pixels = gdk_pixbuf_get_pixels (src);
      guchar *pixdst;
      guchar *pixsrc;
      gint    j;

      for (i = height; --i >= 0; )
        {
          pixdst = dst_pixels + i * dst_row_stride;
          pixsrc = src_pixels + i * src_row_stride;
      
          for (j = width; j > 0; --j)
            {
              *pixdst++ = lighten_channel (*pixsrc++);
              *pixdst++ = lighten_channel (*pixsrc++);
              *pixdst++ = lighten_channel (*pixsrc++);

              if (G_LIKELY (has_alpha))
                *pixdst++ = *pixsrc++;
            }
        }
    }

  return dst;
}



/**
 * exo_gdk_pixbuf_scale_down:
 * @source       : the source #GdkPixbuf.
 * @aspect_ratio : %TRUE to preserve aspect ratio.
 * @dest_width   : the max width for the result.
 * @dest_height  : the max height for the result.
 *
 * Scales down the @source to fit into the given @width and
 * @height. If @aspect_ratio is %TRUE then the aspect ratio
 * of @source will be preserved.
 *
 * If @width is larger than the width of @source and @height
 * is larger than the height of @source, a reference to
 * @source will be returned, as it's unneccesary then to
 * scale down.
 *
 * The caller is responsible to free the returned #GdkPixbuf
 * using g_object_unref() when no longer needed.
 *
 * Return value: the resulting #GdkPixbuf.
 *
 * Since: 0.3.1.1
 **/
GdkPixbuf*
exo_gdk_pixbuf_scale_down (GdkPixbuf *source,
                           gboolean   aspect_ratio,
                           gint       dest_width,
                           gint       dest_height)
{
  gdouble wratio;
  gdouble hratio;
  gint    source_width;
  gint    source_height;

  g_return_val_if_fail (GDK_IS_PIXBUF (source), NULL);
  g_return_val_if_fail (dest_width > 0, NULL);
  g_return_val_if_fail (dest_height > 0, NULL);

  source_width = gdk_pixbuf_get_width (source);
  source_height = gdk_pixbuf_get_height (source);

  /* check if we need to scale */
  if (G_UNLIKELY (source_width <= dest_width && source_height <= dest_height))
    return g_object_ref (G_OBJECT (source));

  /* check if aspect ratio should be preserved */
  if (G_LIKELY (aspect_ratio))
    {
      /* calculate the new dimensions */
      wratio = (gdouble) source_width  / (gdouble) dest_width;
      hratio = (gdouble) source_height / (gdouble) dest_height;

      if (hratio > wratio)
        dest_width  = rint (source_width / hratio);
      else
        dest_height = rint (source_height / wratio);
    }

  return gdk_pixbuf_scale_simple (source, MAX (dest_width, 1), MAX (dest_height, 1), GDK_INTERP_BILINEAR);
}



/**
 * exo_gdk_pixbuf_scale_ratio:
 * @source    : The source #GdkPixbuf.
 * @dest_size : The target size in pixel.
 *
 * Scales @source to @dest_size while preserving the aspect ratio of
 * @source.
 *
 * Return value: A newly created #GdkPixbuf.
 **/
GdkPixbuf*
exo_gdk_pixbuf_scale_ratio (GdkPixbuf *source,
                            gint       dest_size)
{
  gdouble wratio;
  gdouble hratio;
  gint    source_width;
  gint    source_height;
  gint    dest_width;
  gint    dest_height;

  g_return_val_if_fail (GDK_IS_PIXBUF (source), NULL);
  g_return_val_if_fail (dest_size > 0, NULL);

  source_width  = gdk_pixbuf_get_width  (source);
  source_height = gdk_pixbuf_get_height (source);

  wratio = (gdouble) source_width  / (gdouble) dest_size;
  hratio = (gdouble) source_height / (gdouble) dest_size;

  if (hratio > wratio)
    {
      dest_width  = rint (source_width / hratio);
      dest_height = dest_size;
    }
  else
    {
      dest_width  = dest_size;
      dest_height = rint (source_height / wratio);
    }

  return gdk_pixbuf_scale_simple (source, MAX (dest_width, 1), MAX (dest_height, 1), GDK_INTERP_BILINEAR);
}



#define __EXO_GDK_PIXBUF_EXTENSIONS_C__
#include <exo/exo-aliasdef.c>
