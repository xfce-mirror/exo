#!/usr/bin/env python
#
# iconview.py - Simple example to demonstrate the usage of
#               exo.IconView, exo.CellRendererEllipsizedText
#               and exo.CellRendererIcon classes.
#
# $Id$
# vim:set ts=4 sw=4 et ai syntax=python:
#
# Copyright (c) 2006 Written by Benedikt Meurer <benny@xfce.org>.
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


class IconStore(gtk.ListStore):
    def __init__(self):
        gtk.ListStore.__init__(self, gobject.TYPE_STRING, gobject.TYPE_STRING)
        self.append(['gnome-dev-battery', 'Battery'])
        self.append(['gnome-dev-cdrom', 'CD-ROM'])
        self.append(['gnome-dev-floppy', 'Floppy'])
        self.append(['gnome-dev-ipod', 'iPod'])
        self.append(['gnome-dev-keyboard', 'Keyboard'])
        self.append(['gnome-dev-printer', 'Printer'])


class IconWindow(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
        self.connect('destroy', gtk.main_quit)
        self.set_default_size(400, 300)
        self.set_title('IconView Demo')

        vbox = gtk.VBox(False, 12)
        vbox.set_border_width(6)
        self.add(vbox)
        vbox.show()

        swin = gtk.ScrolledWindow()
        swin.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        swin.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        vbox.pack_start(swin, True, True, 0)
        swin.show()

        view = exo.IconView()
        view.set_model(IconStore())
        swin.add(view)
        view.show()

        renderer = exo.CellRendererIcon()
        view.pack_start(renderer, True)
        view.add_attribute(renderer, 'icon', 0)

        renderer = exo.CellRendererEllipsizedText()
        renderer.set_property('follow-state', True)
        renderer.set_property('xalign', 0.5)
        view.pack_start(renderer, False)
        view.add_attribute(renderer, 'text', 1)

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


window = IconWindow()
window.run()
