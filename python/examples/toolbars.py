#!/usr/bin/env python
#
# toolbars.py - Simple example to demonstrate the usage of the editable
#               toolbars framework in libexol.
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


class ToolbarsStore(exo.ToolbarsModel):
    def __init__(self):
        exo.ToolbarsModel.__init__(self)
        self.set_actions(['up', 'down', 'forward', 'back', 'help', 'home'])
        self.load_from_file('toolbars.ui')



class ToolbarsView(exo.ToolbarsView):
    def __init__(self, manager):
        exo.ToolbarsView.__init__(self, manager, ToolbarsStore())
        self.connect('customize', lambda self: self.customize())


    def customize(self):
        self.set_editing(True)

        dialog = exo.ToolbarsEditorDialog(self.get_ui_manager(), self.get_model())
        dialog.set_title('Toolbar Editor')
        dialog.set_transient_for(self.get_toplevel())
        dialog.connect('destroy', lambda dialog: self.set_editing(False))
        dialog.show()



class ToolbarsWindow(gtk.Window):
    def __init__(self):
        gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
        self.connect('destroy', lambda self: gtk.main_quit())
        self.set_default_size(400, 300)
        self.set_title('Toolbars example')

        group = gtk.ActionGroup('toolbars-window')
        group.add_actions([
              ('up', gtk.STOCK_GO_UP, 'Up', '<Control>u', None,
               lambda btn, self: self.label.set_label('<big>Up</big>')),
              ('down', gtk.STOCK_GO_DOWN, 'Down', '<Control>d', None,
               lambda btn, self: self.label.set_label('<big>Down</big>')),
              ('forward', gtk.STOCK_GO_FORWARD, 'Forward', '<Control>f', None,
               lambda btn, self: self.label.set_label('<big>Forward</big>')),
              ('back', gtk.STOCK_GO_BACK, 'Back', '<Control>b', None,
               lambda btn, self: self.label.set_label('<big>Back</big>')),
              ('help', gtk.STOCK_HELP, 'Help me', '<Control>h', None,
               lambda btn, self: self.label.set_label('<big>Help ME!</big>')),
              ('home', gtk.STOCK_HOME, 'Home', '<Control>a', None,
               lambda btn, self: self.label.set_label('<big>Anybody\'s Home?</big>'))
        ], self)

        manager = gtk.UIManager()
        manager.insert_action_group(group, 0)
        self.add_accel_group(manager.get_accel_group())

        vbox = gtk.VBox(False, 0)
        self.add(vbox)
        vbox.show()

        view = ToolbarsView(manager)
        vbox.pack_start(view, False, False, 0)
        view.show()

        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        vbox.pack_start(frame, True, True, 0)
        frame.show()

        self.label = gtk.Label()
        self.label.set_use_markup(True)
        frame.add(self.label)
        self.label.show()


    def run(self):
        self.show()
        gtk.main()


window = ToolbarsWindow()
window.run()
