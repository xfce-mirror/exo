# $Id$

INCLUDES = 								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"exo-csource\"					\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

bin_PROGRAMS =								\
	exo-csource

exo_csource_SOURCES =							\
	main.c

exo_csource_CFLAGS =							\
	$(GLIB_CFLAGS)

exo_csource_LDFLAGS =							\
	-no-undefined

exo_csource_LDADD =							\
	$(GLIB_LIBS)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
