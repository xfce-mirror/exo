# $Id$
# vim:set ts=4 sw=4 et ai syntax=python:
#
# Copyright (c) 2005 os-cillation
# Copyright (c) 1998-2002 James Henstridge
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
#

import fnmatch
import glob
import os
import sys

__all__ = ['require']

_pyexo_dir_pattern = 'exo-[0-9].[0-9]'
_pyexo_required_version = None

def _get_available_versions():
    versions = {}
    for dir in sys.path:
        if not dir:
            dir = os.getcwd()

        if not os.path.isdir(dir):
            continue

        # if the dir is a pyexo dir, skip it
        if fnmatch.fnmatchcase(os.path.basename(dir), _pyexo_dir_pattern):
            continue

        for filename in glob.glob(os.path.join(dir, _pyexo_dir_pattern)):
            pathname = os.path.join(dir, filename)

            # skip non directories
            if not os.path.isdir(pathname):
                continue

            # skip empty directories
            if not os.listdir(pathname):
                continue

            if not versions.has_key(filename[-3:]):
                versions[filename[-3:]] = pathname

    return versions


def require(version):
    global _pyexo_required_version

    if _pyexo_required_version != None:
        assert _pyexo_required_version == version, \
               "a different version of exo was already required"
        return

    assert not sys.modules.has_key('exo'), \
           "pyexo.require() must be called before importing exo"

    versions = _get_available_versions()
    assert versions.has_key(version), \
           "required version '%s' not found on system" % version

    # remove any pyexo dirs first ...
    for dir in sys.path:
        if fnmatch.fnmatchcase(os.path.basename(dir), _pyexo_dir_pattern):
            sys.path.remove(dir)

    # prepend the pyexo path ...
    sys.path.insert(0, versions[version])

    _pyexo_required_version = version
