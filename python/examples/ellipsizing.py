#!/usr/bin/env python
#
# ellipsizing.py - Simple example to demonstrate the usage of
#                  exo.EllipsizedLabel and exo.CellRendererEllipsizedText,
#                  which provide text ellipsizing support for Gtk 2.4,
#                  which also works with Gtk 2.5 and above without the
#                  need to change your code.
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


class EllipsizingStore(gtk.ListStore):
    def __init__(self):
        gtk.ListStore.__init__(self, gobject.TYPE_INT, gobject.TYPE_STRING)
        for i in range(20):
            self.append([i, 'Sample Ellipsizing Store Item #%s' % i])


class EllipsizingTree(gtk.TreeView):
    def __init__(self):
        gtk.TreeView.__init__(self, EllipsizingStore())
        self.set_headers_visible(True)
        self.set_rules_hint(True)

        column = gtk.TreeViewColumn('Index')
        renderer = gtk.CellRendererText()
        column.pack_start(renderer, True)
        column.add_attribute(renderer, 'text', 0)
        self.append_column(column)

        column = gtk.TreeViewColumn('Description')
        renderer = exo.CellRendererEllipsizedText()
        renderer.set_property('ellipsize', exo.PANGO_ELLIPSIZE_END)
        renderer.set_property('ellipsize-set', True)
        column.pack_start(renderer, True)
        column.add_attribute(renderer, 'text', 1)
        self.append_column(column)
        self.set_expander_column(column)


class EllipsizingWindow(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
        self.connect('destroy', gtk.main_quit)
        self.set_default_size(400, 300)
        self.set_title('Ellipsizing Demo')

        vbox = gtk.VBox(False, 12)
        vbox.set_border_width(12)
        self.add(vbox)
        vbox.show()

        label = exo.EllipsizedLabel('Resize this window to see ' \
                                    'the ellipsizing effects')
        label.set_ellipsize(exo.PANGO_ELLIPSIZE_MIDDLE)
        vbox.pack_start(label, False, True, 0)
        label.show()

        swin = gtk.ScrolledWindow()
        swin.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
        swin.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        vbox.pack_start(swin, True, True, 0)
        swin.show()

        tree = EllipsizingTree()
        swin.add(tree)
        tree.show()

        bbox = gtk.HButtonBox()
        bbox.set_layout(gtk.BUTTONBOX_END)
        vbox.pack_start(bbox, False, True, 0)
        bbox.show()

        button = gtk.Button(None, gtk.STOCK_QUIT)
        button.connect('clicked', gtk.main_quit)
        bbox.pack_start(button, False, False, 0)
        button.show()

    def run(self):
        self.show()
        gtk.main()


window = EllipsizingWindow()
window.run()
