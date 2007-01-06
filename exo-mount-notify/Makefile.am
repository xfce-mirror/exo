# $Id$

INCLUDES = 								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"exo-mount-notify\"				\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

libexec_PROGRAMS =							\
	exo-mount-notify-0.3

exo_mount_notify_0_3_SOURCES =						\
	main.c

exo_mount_notify_0_3_CFLAGS =						\
	$(GTK_CFLAGS)							\
	$(LIBNOTIFY_CFLAGS)						\
	$(LIBXFCE4UTIL_CFLAGS)

exo_mount_notify_0_3_LDFLAGS =						\
	-no-undefined

exo_mount_notify_0_3_LDADD =						\
	$(GTK_LIBS)							\
	$(LIBNOTIFY_LIBS)						\
	$(LIBXFCE4UTIL_LIBS)

EXTRA_DIST =								\
	README

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
