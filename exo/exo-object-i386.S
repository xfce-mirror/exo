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

	.file		"exo-object-i386.S"
gcc2_compiled.:
	.text


	.align		16
	.globl		exo_object_ref
	.type		exo_object_ref, @function
exo_object_ref:
	movl		4(%esp), %eax
	leal		4(%eax), %edx
	lock; incl	(%edx)
	ret
	.size		exo_object_ref, .-exo_object_ref


