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

	.file		"exo-noop-i386.S"
gcc2_compiled.:
	.text


	.globl		exo_noop_zero
	.set		exo_noop_zero, exo_noop_null


	.globl		exo_noop_true
	.set		exo_noop_true, exo_noop_one


	.globl		exo_noop_false
	.set		exo_noop_false, exo_noop_null


	.align		16
	.globl		exo_noop
	.type		exo_noop, @function
exo_noop:
	ret
	.size		exo_noop, .-exo_noop


	.align		16
	.globl		exo_noop_one
	.type		exo_noop_one, @function
exo_noop_one:
	movl		$1, %eax
	ret
	.size		exo_noop_one, .-exo_noop_one


	.align		16
	.globl		exo_noop_null
	.type		exo_noop_null, @function
exo_noop_null:
	xorl		%eax, %eax
	ret
	.size		exo_noop_null, .-exo_noop_null

