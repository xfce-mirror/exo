# $Id$

INCLUDES = 								\
	-I$(top_builddir)						\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"exo-support\"

noinst_LTLIBRARIES =							\
	libexo-support.la

libexo_support_la_SOURCES =						\
	exo-support.h							\
	xfce-heading.c							\
	xfce-heading.h							\
	xfce-titled-dialog.c						\
	xfce-titled-dialog.h

libexo_support_la_CFLAGS =						\
	$(GTK_CFLAGS)							\
	$(LIBXFCE4UTIL_CFLAGS)

libexo_support_la_LIBADD =						\
	$(GTK_LIBS)							\
	$(LIBXFCE4UTIL_LIBS)

EXTRA_DIST =								\
	README

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
