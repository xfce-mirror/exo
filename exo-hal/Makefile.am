# $Id$

INCLUDES = 								\
	-I$(top_srcdir)							\
	-DDBUS_API_SUBJECT_TO_CHANGE					\
	-DG_LOG_DOMAIN=\"exo-hal\"					\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

libexohalincludedir = $(includedir)/exo-0.3/exo-hal
libexohalinclude_HEADERS = exo-hal.h

lib_LTLIBRARIES =							\
	libexo-hal-0.3.la

libexo_hal_0_3_la_SOURCES =						\
	$(libexohalinclude_HEADERS)					\
	exo-hal.c

libexo_hal_0_3_la_CFLAGS =						\
	$(GLIB_CFLAGS)							\
	$(HAL_CFLAGS)

libexo_hal_0_3_la_LDFLAGS =						\
	-export-dynamic							\
	-version-info $(LIBEXO_VERINFO)					\
	-export-symbols-regex "^[^_].*"					\
	-no-undefined

libexo_hal_0_3_la_LIBADD =						\
	$(GLIB_LIBS)							\
	$(HAL_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = exo-hal-0.3.pc

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
