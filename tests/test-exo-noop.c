/* $Id$ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <exo/exo.h>



int
main (int argc, char **argv)
{
  /* verify the results */
  assert (exo_noop_one () == 1);
  assert (exo_noop_zero () == 0);
  assert (exo_noop_null () == NULL);
  assert (exo_noop_true () == TRUE);
  assert (exo_noop_false () == FALSE);

  return EXIT_SUCCESS;
}
