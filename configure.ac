dnl $Id: configure.ac,v 1.2 2004/09/17 09:48:24 bmeurer Exp $
dnl
dnl Copyright (c) 2004 os-cillation.
dnl All rights reserved.
dnl
dnl Written by Benedikt Meurer <benny@xfce.org>.
dnl

dnl Version information
m4_define([libexo_version_major], [0])
m4_define([libexo_version_minor], [1])
m4_define([libexo_version_micro], [0])
m4_define([libexo_version], [libexo_version_major.libexo_version_minor.libexo_version_micro])

dnl Initialize autoconf
AC_COPYRIGHT([Copyright (c) 2004 os-cillation.
All rights reserved.
        
Written by Benedikt Meurer <benny@xfce.org>.])
AC_INIT([libexo], [libexo_version], [benny@xfce.org])
AC_PREREQ([2.50])
AC_REVISION([$Id: configure.ac,v 1.2 2004/09/17 09:48:24 bmeurer Exp $])

dnl Initialize automake
LIBEXO_VERSION=libexo_version
AM_INIT_AUTOMAKE([libexo], [$LIBEXO_VERSION])
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE

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
AC_CHECK_HEADERS([errno.h locale.h math.h md5.h openssl/md5.h string.h \
                  sys/resource.h sys/stat.h sys/time.h sys/types.h \
                  time.h])

dnl check for Message Digest functions
AC_SEARCH_LIBS([MD5Init], [md c], \
               [AC_DEFINE([HAVE_MD5INIT], [1], [Define if MD5Init present])])
AC_SEARCH_LIBS([MD5_Init], [crypto], \
               [AC_DEFINE([HAVE_MD5_INIT], [1], [Define if MD5_Init present])])

dnl Check for required packages
BM_DEPEND([DBUS], [dbus-glib-1], [0.22])
BM_DEPEND([GTK], [gtk+-2.0], [2.4.0])
BM_DEPEND([LIBXFCEGUI4], [libxfcegui4-1.0], [4.1.24])

dnl Check for debugging support
BM_DEBUG_SUPPORT

dnl Check whether to build a final version
AC_ARG_ENABLE([final],
AC_HELP_STRING([--enable-final], [Build final]),
  [], [enable_final=no])
AC_MSG_CHECKING([whether to build final])
if test x"$enable_final" = x"no"; then
  AC_MSG_RESULT([no])
else
  CPPFLAGS="$CPPFLAGS -DG_DISABLE_CHECKS -DG_DISABLE_ASSERT"
  CPPFLAGS="$CPPFLAGS -DG_DISABLE_CAST_CHECKS"
  AC_MSG_RESULT([yes])
fi

AC_OUTPUT([
Makefile
exo/exo-0.1.pc
exo/Makefile
])
