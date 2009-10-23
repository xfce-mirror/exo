#!/usr/bin/env python
#
# preferred-applications.py - Example demonstrating the usage of the preferred
#                             applications and the URL modules in libexo.
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


class PreferredWindow(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
        self.connect('destroy', gtk.main_quit)
        self.set_title("Preferred Applications")

        self.vbox = gtk.VBox(False, 12);
        self.vbox.set_border_width(12)
        self.add(self.vbox)
        self.vbox.show()

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        self.vbox.pack_start(frame, False, False, 0)
        frame.show()

        label = gtk.Label("<b>Preferred Applications</b>")
        label.set_use_markup(True)
        frame.set_label_widget(label)
        label.show()

        vbox = gtk.VBox(False, 12)
        vbox.set_border_width(12)
        frame.add(vbox)
        vbox.show()
        
        button = gtk.Button("Open Preferred Web Browser")
        button.connect('clicked', lambda btn: exo.execute_preferred_application("WebBrowser"))
        vbox.pack_start(button, False, False, 0)
        button.show()

        button = gtk.Button("Open Preferred Mail Reader")
        button.connect('clicked', lambda btn: exo.execute_preferred_application("MailReader"))
        vbox.pack_start(button, False, False, 0)
        button.show()

        button = gtk.Button("Open Preferred Terminal Emulator")
        button.connect('clicked', lambda btn: exo.execute_preferred_application("TerminalEmulator"))
        vbox.pack_start(button, False, False, 0)
        button.show()

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        self.vbox.pack_start(frame, False, False, 0)
        frame.show()

        label = gtk.Label("<b>Executing in Terminal</b>")
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

        entry_cmd = gtk.Entry()
        entry_cmd.set_text("bash")
        hbox.pack_start(entry_cmd, True, True, 0)
        entry_cmd.show()

        button = gtk.Button("Run in Terminal")
        button.connect('clicked', lambda btn: exo.execute_terminal_shell(entry_cmd.get_text()))
        hbox.pack_start(button, False, False, 0)
        button.show()

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        self.vbox.pack_start(frame, False, False, 0)
        frame.show()

        label = gtk.Label("<b>Opening URLs</b>")
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

        entry_url = gtk.Entry()
        entry_url.set_text("http://www.xfce.org")
        hbox.pack_start(entry_url, True, True, 0)
        entry_url.show()

        button = gtk.Button("Open URL")
        button.connect('clicked', lambda btn: exo.url_show(entry_url.get_text()))
        hbox.pack_start(button, False, False, 0)
        button.show()

        hbox = gtk.HButtonBox()
        hbox.set_layout(gtk.BUTTONBOX_END)
        self.vbox.pack_end(hbox, False, False, 0)
        hbox.show()

        button = gtk.Button(None, gtk.STOCK_CLOSE)
        button.connect('clicked', gtk.main_quit)
        hbox.pack_end(button, False, False, 0)
        button.show()

    def run(self):
        self.show()
        gtk.main()


window = PreferredWindow()
window.run()

# vim:set ts=4 sw=4 et ai syntax=python:
