#!/usr/bin/env python
#
# mime.py - Simple example to demonstrate the usage of the mime
#           types framework in libexo.
#
# $Id$
# vim:set ts=4 sw=4 et ai syntax=python:
#
# Copyright (c) 2005 os-cillation
#
# Written by Benedikt Meurer <benny@xfce.org>.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#                                                                             
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#                                                                             
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

import pygtk
pygtk.require('2.0')
import gobject
import gtk

import pyexo
pyexo.require('0.3')
import exo


# query the shared MIME database instance
database = exo.mime_database_get_default()

# lookup MIME types for various files
for file in ['Makefile', 'README', 'mime.py', 'toolbars.ui']:
	info = database.get_info_for_file(file)
	print "MIME type of " + file + " is " + info.get_name() + " (" + info.get_comment() + ")"

