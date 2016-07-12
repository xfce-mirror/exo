
AM_CPPFLAGS = 								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"exo-open\"					\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

bin_PROGRAMS =								\
	exo-open

exo_open_SOURCES =							\
	main.c

exo_open_CFLAGS =							\
	$(GTK3_CFLAGS)							\
	$(LIBXFCE4UTIL_CFLAGS)						\
	$(GIO_CFLAGS)							\
	$(GIO_UNIX_CFLAGS)

exo_open_LDFLAGS =							\
	-no-undefined

exo_open_LDADD =							\
	$(GTK3_LIBS)							\
	$(LIBXFCE4UTIL_LIBS)						\
	$(GIO_LIBS)							\
	$(GIO_UNIX_LIBS)						\
	$(top_builddir)/exo/libexo-2.la

desktopdir = $(datadir)/applications
desktop_in_files =							\
	exo-file-manager.desktop.in					\
	exo-mail-reader.desktop.in					\
	exo-terminal-emulator.desktop.in				\
	exo-web-browser.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =								\
	$(desktop_in_files)

DISTCLEANFILES =							\
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
