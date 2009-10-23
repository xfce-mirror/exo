# $Id$

INCLUDES = 								\
	-I$(top_srcdir)							\
	-DDBUS_API_SUBJECT_TO_CHANGE					\
	-DG_LOG_DOMAIN=\"exo-mount\"					\
	-DLIBEXECDIR=\"$(libexecdir)\"					\
	-DLIBEXO_VERSION_API=\"$(LIBEXO_VERSION_API)\"			\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	-DPATH_MOUNT=\"$(PATH_MOUNT)\"					\
	-DPATH_UMOUNT=\"$(PATH_UMOUNT)\"				\
	-DPATH_EJECT=\"$(PATH_EJECT)\"

defaultsdir = $(sysconfdir)/xdg/xfce4
defaults_DATA = mount.rc

bin_PROGRAMS =								\
	exo-mount

exo_mount_SOURCES =							\
	main.c

if HAVE_HAL
exo_mount_SOURCES +=							\
	exo-mount-hal.c							\
	exo-mount-hal.h
endif

exo_mount_CFLAGS =							\
	$(GTK_CFLAGS)							\
	$(HAL_CFLAGS)							\
	$(LIBXFCE4UTIL_CFLAGS)						\
	$(GIO_CFLAGS)							\
	$(GIO_UNIX_CFLAGS)

exo_mount_LDFLAGS =							\
	-no-undefined

exo_mount_DEPENDENCIES =						\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_API).la \
	$(top_builddir)/exo-hal/libexo-hal-$(LIBEXO_VERSION_API).la

exo_mount_LDADD =							\
	$(GTK_LIBS)							\
	$(HAL_LIBS)							\
	$(LIBXFCE4UTIL_LIBS)						\
	$(GIO_LIBS)							\
	$(GIO_UNIX_LIBS)						\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_API).la		\
	$(top_builddir)/exo-hal/libexo-hal-$(LIBEXO_VERSION_API).la

# install apppropriate symlinks
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	-( cd $(DESTDIR)$(bindir) ; ln -sf exo-mount exo-eject )
	-( cd $(DESTDIR)$(bindir) ; ln -sf exo-mount exo-unmount )

EXTRA_DIST =								\
	$(defaults_DATA)					\
	README

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
