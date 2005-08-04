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
	.globl		exo_object_new
	.type		exo_object_new, @function
exo_object_new:
	/* just jump to g_type_create_instance passing
	 * the full stack to it.
	 */
	jmp		g_type_create_instance
	.size		exo_object_new, .-exo_object_new


	.align		16
	.globl		exo_object_ref
	.type		exo_object_ref, @function
exo_object_ref:
	movl		4(%esp), %eax
	leal		4(%eax), %edx
	lock; incl	(%edx)
	ret
	.size		exo_object_ref, .-exo_object_ref


	.align		16
	.globl		exo_object_unref
	.type		exo_object_unref, @function
exo_object_unref:
	/* decrement the ref_count by one and remember 
	 * the previous value in %eax.
	 */
	movl		4(%esp), %eax
	leal		4(%eax), %edx
	movl		$-1, %eax
	lock; xaddl	%eax, (%edx)

	/* if the previous value was not 1 then we
	 * simply return from here.
	 */
	decl		%eax
	jz		.L1
	ret
.L1:
	/* else we first call the finalize method
	 * (which has offset 4 in the class struct).
	 */
	movl		4(%esp), %edx
	pushl		%edx
	movl		(%edx), %eax
	call		*4(%eax)
	addl		$4, %esp

	/* and afterwards leave it to the function
	 * g_type_free_instance to clean up the
	 * stack, which is possible, because that
	 * function takes exactly the same parameters.
	 */
	jmp		g_type_free_instance
	.size		exo_object_unref, .-exo_object_unref

