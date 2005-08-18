/* $Id$ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>.
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

#include <exo/exo-noop.h>
#include <exo/exo-alias.h>



/**
 * exo_noop:
 *
 * This function has no effect. It does nothing but
 * returning instantly. It is mostly useful in
 * situations that require a function to be called,
 * but that function does not need to do anything
 * useful.
 **/
void
exo_noop (void)
{
}



/**
 * exo_noop_one:
 *
 * This function has no effect but simply returns
 * the integer value %1. It is mostly useful in
 * situations where you just need a function that
 * returns %1, but don't want to perform any other
 * actions.
 *
 * Return value: the integer value %1.
 **/
gint
exo_noop_one (void)
{
  return 1;
}



/**
 * exo_noop_zero:
 *
 * This function has no effect but simply returns
 * the integer value %0. It is mostly useful in
 * situations where you just need a function that
 * returns %0, but don't want to perform any other
 * actions.
 *
 * Return value: the integer value %0.
 **/
gint
exo_noop_zero (void)
{
  return 0;
}



/**
 * exo_noop_null:
 *
 * This function has no effect but simply returns
 * a %NULL pointer. It is mostly useful in
 * situations where you just need a function that
 * returns %NULL, but don't want to perform any
 * other actions.
 *
 * Return value: a %NULL pointer.
 **/
gpointer
exo_noop_null (void)
{
  return NULL;
}



/**
 * exo_noop_true:
 *
 * This function has no effect, but simply returns
 * the boolean value %TRUE. It is mostly useful in
 * situations where you just need a function that
 * returns %TRUE, but don't want to perform any
 * other actions.
 *
 * Return value: the boolean value %TRUE.
 **/
gboolean
exo_noop_true (void)
{
  return TRUE;
}



/**
 * exo_noop_false:
 *
 * This function has no effect, but simply returns
 * the boolean value %FALSE. It is mostly useful in
 * situations where you just need a function that
 * returns %FALSE, but don't want to perform any
 * other actions.
 *
 * Return value: the boolean value %FALSE.
 **/
gboolean
exo_noop_false (void)
{
  return FALSE;
}



#define __EXO_NOOP_C__
#include <exo/exo-aliasdef.c>

