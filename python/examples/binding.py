#!/usr/bin/env python
#
# binding.py - Example demonstrating the usage of exo.Binding and
#              exo.MutualBinding to automatically synchronize GObject
#              properties.
#
#
# Copyright (c) 2005-2006 os-cillation
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


class BindingWindow(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
        self.connect('destroy', gtk.main_quit)
        self.set_default_size(400, 300)

        self.vbox = gtk.VBox(False, 12);
        self.vbox.set_border_width(12)
        self.add(self.vbox)
        self.vbox.show()

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        self.vbox.pack_start(frame, False, False, 0)
        frame.show()

        label = gtk.Label("<b>Binding</b>")
        label.set_use_markup(True)
        frame.set_label_widget(label)
        label.show()

        vbox = gtk.VBox(False, 12)
        vbox.set_border_width(12)
        frame.add(vbox)
        vbox.show()

        hbox = gtk.HBox(False, 6)
        vbox.pack_start(hbox, False, False, 0)
        hbox.show()

        label = gtk.Label("Title:")
        hbox.pack_start(label, False, False, 0)
        label.show()

        entry = gtk.Entry()
        entry.set_text("Sample Title")
        self.binding_title = exo.Binding(entry, "text", self, "title")
        hbox.pack_start(entry, True, True, 0)
        entry.show()

        hbox = gtk.HBox(False, 6)
        vbox.pack_start(hbox, False, False, 0)
        hbox.show()

        self.button_bind = gtk.Button("Bind")
        self.button_bind.set_sensitive(False)
        self.button_bind.connect('clicked', lambda btn: self.bind_title(entry))
        hbox.pack_start(self.button_bind, False, False, 0)
        self.button_bind.show()

        self.button_unbind = gtk.Button("Unbind")
        self.button_unbind.connect('clicked', lambda btn: self.unbind_title())
        hbox.pack_start(self.button_unbind, False, False, 0)
        self.button_unbind.show()

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        self.vbox.pack_start(frame, False, False, 0)
        frame.show()

        label = gtk.Label("<b>Mutual Binding</b>")
        label.set_use_markup(True)
        frame.set_label_widget(label)
        label.show()

        vbox = gtk.VBox(False, 12)
        vbox.set_border_width(12)
        frame.add(vbox)
        vbox.show()

        hbox = gtk.HBox(False, 6)
        vbox.pack_start(hbox, False, False, 0)
        hbox.show()

        label = gtk.Label("Text1:")
        hbox.pack_start(label, False, False, 0)
        label.show()

        entry1 = gtk.Entry()
        entry1.set_text("Sample Text")
        hbox.pack_start(entry1, True, True, 0)
        entry1.show()

        hbox = gtk.HBox(False, 6)
        vbox.pack_start(hbox, False, False, 0)
        hbox.show()

        label = gtk.Label("Text2:")
        hbox.pack_start(label, False, False, 0)
        label.show()

        entry2 = gtk.Entry()
        hbox.pack_start(entry2, True, True, 0)
        entry2.show()

        exo.MutualBinding(entry1, "text", entry2, "text")

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        self.vbox.pack_start(frame, False, False, 0)
        frame.show()

        label = gtk.Label("<b>Binding With Negation</b>")
        label.set_use_markup(True)
        frame.set_label_widget(label)
        label.show()

        vbox = gtk.VBox(False, 12)
        vbox.set_border_width(12)
        frame.add(vbox)
        vbox.show()

        hbox = gtk.HBox(False, 6)
        vbox.pack_start(hbox, False, False, 0)
        hbox.show()

        button1 = gtk.CheckButton("Say yes")
        hbox.pack_start(button1, False, False, 0)
        button1.show()

        button2 = gtk.CheckButton("Say no")
        hbox.pack_start(button2, False, False, 0)
        button2.show()

        exo.MutualBindingWithNegation(button1, "active", button2, "active")

        hbox = gtk.HButtonBox()
        hbox.set_layout(gtk.BUTTONBOX_END)
        self.vbox.pack_end(hbox, False, False, 0)
        hbox.show()

        button = gtk.Button(None, gtk.STOCK_CLOSE)
        button.connect('clicked', gtk.main_quit)
        hbox.pack_end(button, False, False, 0)
        button.show()

    def unbind_title(self):
        self.binding_title.unbind()
        self.button_bind.set_sensitive(True)
        self.button_unbind.set_sensitive(False)

    def bind_title(self, entry):
        self.binding_title = exo.Binding(entry, "text", self, "title")
        self.button_unbind.set_sensitive(True)
        self.button_bind.set_sensitive(False)

    def run(self):
        self.show()
        gtk.main()


window = BindingWindow()
window.run()

# vim:set ts=4 sw=4 et ai syntax=python:
