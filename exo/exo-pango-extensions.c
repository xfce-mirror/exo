/* $Id$ */
/*-
 * Copyright (c) 2004 os-cillation e.K.
 * Copyright (c) 2000 Anders Carlsson <andersca@gnu.org>
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

#include <exo/exo-pango-extensions.h>
#include <exo/exo-string.h>



#define ELLIPSIS "..."



#ifndef PANGO_TYPE_ELLIPSIZE_MODE
static int
measure_string_width (const char  *string,
                      PangoLayout *layout)
{
	int width;
	
	pango_layout_set_text (layout, string, -1);
	pango_layout_get_pixel_size (layout, &width, NULL);

	return width;
}



static void
compute_character_widths (const char   *string,
                          PangoLayout  *layout,
                          int          *char_len_return,
                          int         **widths_return,
                          int         **cuts_return)
{
	int *widths;
	int *offsets;
	int *cuts;
	int char_len;
	int byte_len;
	const char *p;
	int i;
	PangoLayoutIter *iter;
	PangoLogAttr *attrs;
	
#define BEGINS_UTF8_CHAR(x) (((x) & 0xc0) != 0x80)
	
	char_len = g_utf8_strlen (string, -1);
	byte_len = strlen (string);
	
	widths = g_new (int, char_len);
	offsets = g_new (int, byte_len);

	/* Create a translation table from byte index to char offset */
	p = string;
	i = 0;
	while (*p) {
		int byte_index = p - string;
		
		if (BEGINS_UTF8_CHAR (*p)) {
			offsets[byte_index] = i;
			++i;
		} else {
			offsets[byte_index] = G_MAXINT; /* segv if we try to use this */
		}
		
		++p;
	}

	/* Now fill in the widths array */
	pango_layout_set_text (layout, string, -1);
	
	iter = pango_layout_get_iter (layout);

	do {
		PangoRectangle extents;
		int byte_index;

		byte_index = pango_layout_iter_get_index (iter);

		if (byte_index < byte_len) {
			pango_layout_iter_get_char_extents (iter, &extents);
			
			g_assert (BEGINS_UTF8_CHAR (string[byte_index]));
			g_assert (offsets[byte_index] < char_len);
			
			widths[offsets[byte_index]] = PANGO_PIXELS (extents.width);
		}
		
	} while (pango_layout_iter_next_char (iter));

	pango_layout_iter_free (iter);

	g_free (offsets);
	
	*widths_return = widths;

	/* Now compute character offsets that are legitimate places to
	 * chop the string
	 */
	attrs = g_new (PangoLogAttr, char_len + 1);
	
	pango_get_log_attrs (string, byte_len, -1,
			     pango_context_get_language (
				     pango_layout_get_context (layout)),
			     attrs,
			     char_len + 1);

	cuts = g_new (int, char_len);
	i = 0;
	while (i < char_len) {
		cuts[i] = attrs[i].is_cursor_position;

		++i;
	}

	g_free (attrs);

	*cuts_return = cuts;

	*char_len_return = char_len;
}



static char *
ellipsize_start (const char *string, PangoLayout *layout, int width)
{
	int resulting_width;
	int *cuts;
	int *widths;
	int char_len;
	const char *p;
	int truncate_offset;

	/* Zero-length string can't get shorter - catch this here to
	 * avoid expensive calculations
	 */
	if (*string == '\0')
		return g_strdup ("");

	/* I'm not sure if this short-circuit is a net win; it might be better
	 * to just dump this, and always do the compute_character_widths() etc.
	 * down below.
	 */
	resulting_width = measure_string_width (string, layout);

	if (resulting_width <= width) {
		/* String is already short enough. */
		return g_strdup (string);
	}

	/* Remove width of an ellipsis */
	width -= measure_string_width (ELLIPSIS, layout);

	if (width < 0) {
		/* No room even for an ellipsis. */
		return g_strdup ("");
	}

	/* Our algorithm involves removing enough chars from the string to bring
	 * the width to the required small size. However, due to ligatures,
	 * combining characters, etc., it's not guaranteed that the algorithm
	 * always works 100%. It's sort of a heuristic thing. It should work
	 * nearly all the time... but I wouldn't put in
	 * g_assert (width of resulting string < width).
	 *
	 * Hmm, another thing that this breaks with is explicit line breaks
	 * in "string"
	 */

	compute_character_widths (string, layout, &char_len, &widths, &cuts);

        for (truncate_offset = 1; truncate_offset < char_len; truncate_offset++) {

        	resulting_width -= widths[truncate_offset];

        	if (resulting_width <= width &&
		    cuts[truncate_offset]) {
			break;
        	}
        }

	g_free (cuts);
	g_free (widths);
	
	p = g_utf8_offset_to_pointer (string, truncate_offset);
	
	return g_strconcat (ELLIPSIS, p, NULL);
}




static char *
ellipsize_end (const char *string, PangoLayout *layout, int width)
{
	int resulting_width;
	int *cuts;
	int *widths;
	int char_len;
	const char *p;
	int truncate_offset;
	char *result;
	
	/* See explanatory comments in ellipsize_start */
	
	if (*string == '\0')
		return g_strdup ("");

	resulting_width = measure_string_width (string, layout);
	
	if (resulting_width <= width) {
		return g_strdup (string);
	}

	width -= measure_string_width (ELLIPSIS, layout);

	if (width < 0) {
		return g_strdup ("");
	}
	
	compute_character_widths (string, layout, &char_len, &widths, &cuts);
	
        for (truncate_offset = char_len - 1; truncate_offset > 0; truncate_offset--) {
        	resulting_width -= widths[truncate_offset];
        	if (resulting_width <= width &&
		    cuts[truncate_offset]) {
			break;
        	}
        }

	g_free (cuts);
	g_free (widths);

	p = g_utf8_offset_to_pointer (string, truncate_offset);
	
	result = g_malloc ((p - string) + strlen (ELLIPSIS) + 1);
	memcpy (result, string, (p - string));
	strcpy (result + (p - string), ELLIPSIS);

	return result;
}




static char *
ellipsize_middle (const char *string, PangoLayout *layout, int width)
{
	int resulting_width;
	int *cuts;
	int *widths;
	int char_len;
	int starting_fragment_byte_len;
	int ending_fragment_byte_index;
	int starting_fragment_length;
	int ending_fragment_offset;
	char *result;
	
	/* See explanatory comments in ellipsize_start */
	
	if (*string == '\0')
		return g_strdup ("");

	resulting_width = measure_string_width (string, layout);
	
	if (resulting_width <= width) {
		return g_strdup (string);
	}

	width -= measure_string_width (ELLIPSIS, layout);

	if (width < 0) {
		return g_strdup ("");
	}
	
	compute_character_widths (string, layout, &char_len, &widths, &cuts);
	
	starting_fragment_length = char_len / 2;
	ending_fragment_offset = starting_fragment_length + 1;
	
	/* Shave off a character at a time from the first and the second half
	 * until we can fit
	 */
	resulting_width -= widths[ending_fragment_offset - 1];
	
	/* depending on whether the original string length is odd or even, start by
	 * shaving off the characters from the starting or ending fragment
	 */
	if (char_len % 2) {
		goto shave_end;
	}

	while (starting_fragment_length > 0 || ending_fragment_offset < char_len) {
		if (resulting_width <= width &&
		    cuts[ending_fragment_offset] &&
		    cuts[starting_fragment_length]) {
			break;
		}

		if (starting_fragment_length > 0) {
			resulting_width -= widths[starting_fragment_length];
			starting_fragment_length--;
		}

	shave_end:
		if (resulting_width <= width &&
		    cuts[ending_fragment_offset] &&
		    cuts[starting_fragment_length]) {
			break;
		}

		if (ending_fragment_offset < char_len) {
			resulting_width -= widths[ending_fragment_offset];
			ending_fragment_offset++;
		}
	}

	g_free (cuts);
	g_free (widths);	
	
	/* patch the two fragments together with an ellipsis */
	result = g_malloc (strlen (string) + strlen (ELLIPSIS) + 1); /* a bit wasteful, no biggie */

	starting_fragment_byte_len = g_utf8_offset_to_pointer (string, starting_fragment_length) - string;
	ending_fragment_byte_index = g_utf8_offset_to_pointer (string, ending_fragment_offset) - string;
	
	memcpy (result, string, starting_fragment_byte_len);
	strcpy (result + starting_fragment_byte_len, ELLIPSIS);
	strcpy (result + starting_fragment_byte_len + strlen (ELLIPSIS), string + ending_fragment_byte_index);

	return result;
}
#endif /* !PANGO_TYPE_ELLIPSIZE_MODE */



/**
 * exo_pango_layout_set_text_ellipsized:
 * @layout      : A #PangoLayout.
 * @string      :
 * @width       :
 * @mode        :
 * 
 * Truncates a string if required to fit in @width and sets it on the
 * layout. Truncation involves removing characters from the start, middle or end
 * respectively and replacing them with "...". Algorithm is a bit
 * fuzzy, won't work 100%.
 *
 * Return value : %TRUE if @string had to be ellipsized to fit into @width, else
 *                %FALSE.
 **/
gboolean
exo_pango_layout_set_text_ellipsized (PangoLayout            *layout,
                                       const char             *string,
                                       int                     width,
                                       ExoPangoEllipsizeMode  mode)
{
#ifdef PANGO_TYPE_ELLIPSIZE_MODE
  g_return_val_if_fail (PANGO_IS_LAYOUT (layout), FALSE);
  g_return_val_if_fail (string != NULL, FALSE);
  g_return_val_if_fail (width >= 0, FALSE);

  pango_layout_set_text (layout, string, -1);
  pango_layout_set_width (layout, PANGO_SCALE * width);
  pango_layout_set_ellipsize (layout, mode);

  return (mode != EXO_PANGO_ELLIPSIZE_NONE);
#else
  gboolean  equals;
  gchar    *result;

  g_return_val_if_fail (PANGO_IS_LAYOUT (layout), FALSE);
  g_return_val_if_fail (string != NULL, FALSE);
  g_return_val_if_fail (width >= 0, FALSE);

  switch (mode)
    {
    case EXO_PANGO_ELLIPSIZE_NONE:
      result = g_strdup (string);
      break;

    case EXO_PANGO_ELLIPSIZE_START:
      result = ellipsize_start (string, layout, width);
      break;

    case EXO_PANGO_ELLIPSIZE_MIDDLE:
      result = ellipsize_middle (string, layout, width);
      break;

    case EXO_PANGO_ELLIPSIZE_END:
      result = ellipsize_end (string, layout, width);
      break;

    default:
      g_assert_not_reached ();
      return FALSE;
    }

  /* check if the string was ellipsized */
  equals = exo_str_is_equal (string, result);

  pango_layout_set_text (layout, result, -1);
  g_free (result);

  return !equals;
#endif
}




