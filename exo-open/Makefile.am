# $Id$

INCLUDES = 								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"exo-open\"					\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

bin_PROGRAMS =								\
	exo-open

exo_open_SOURCES =							\
	main.c

exo_open_CFLAGS =							\
	$(GTK_CFLAGS)							\
	$(LIBXFCE4UTIL_CFLAGS)

exo_open_LDFLAGS =							\
	-no-undefined

exo_open_LDADD =							\
	$(GTK_LIBS)							\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
