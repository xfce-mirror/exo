dnl $Id$
dnl
dnl Copyright (c) 2004-2005 os-cillation.
dnl All rights reserved.
dnl
dnl Written by Benedikt Meurer <benny@xfce.org>.
dnl

dnl Version information
m4_define([libexo_verinfo], [0:1:0])
m4_define([libexo_version_major], [0])
m4_define([libexo_version_minor], [2])
m4_define([libexo_version_micro], [1])
m4_define([libexo_version_tag], [svn])
m4_define([libexo_version], [libexo_version_major().libexo_version_minor().libexo_version_micro()libexo_version_tag()])

dnl Initialize autoconf
AC_COPYRIGHT([Copyright (c) 2004-2005 os-cillation.
All rights reserved.
        
Written by Benedikt Meurer <benny@xfce.org>.])
AC_INIT([exo], [libexo_version()], [benny@xfce.org])
AC_PREREQ([2.50])
AC_REVISION([$Id$])

dnl Initialize automake
AM_INIT_AUTOMAKE([AC_PACKAGE_TARNAME()], [AC_PACKAGE_VERSION()])
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE

dnl libtool versioning
LIBEXO_VERINFO=libexo_verinfo()
AC_SUBST([LIBEXO_VERINFO])

dnl Subst libexo version
LIBEXO_VERSION_MAJOR=libexo_version_major()
LIBEXO_VERSION_MINOR=libexo_version_minor()
LIBEXO_VERSION_MICRO=libexo_version_micro()
AC_SUBST([LIBEXO_VERSION_MAJOR])
AC_SUBST([LIBEXO_VERSION_MINOR])
AC_SUBST([LIBEXO_VERSION_MICRO])

dnl check for UNIX variants
AC_AIX
AC_ISC_POSIX
AC_MINIX

dnl check for basic programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

dnl check for standard header files
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h fcntl.h locale.h math.h md5.h string.h \
                  sys/resource.h sys/stat.h sys/time.h sys/types.h \
                  time.h])

dnl check for Message Digest functions
AC_SEARCH_LIBS([MD5Init], [md c], \
               [AC_DEFINE([HAVE_MD5INIT], [1], [Define if MD5Init present])])

dnl Check for i18n support
BM_I18N([libexo], [de en_GB])

dnl Check for required packages
BM_DEPEND([DBUS], [dbus-glib-1], [0.22])
BM_DEPEND([GTK], [gtk+-2.0], [2.4.0])
BM_DEPEND([LIBXFCEGUI4], [libxfcegui4-1.0], [4.2.0])

dnl Check for session management libraries
BM_LIBSM

dnl Check for X11 libraries
BM_LIBX11_REQUIRE

dnl Check for gtk-doc
GTK_DOC_CHECK([1.0])

dnl Check for debugging support
BM_DEBUG_SUPPORT

AC_OUTPUT([
Makefile
exo.spec
docs/version.xml
docs/Makefile
exo/exo-0.2.pc
exo/exo-config.h
exo/Makefile
po/Makefile.in
])
