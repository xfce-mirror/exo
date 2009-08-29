# $Id$

INCLUDES = 								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"exo-gio-module\"				\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	-DLIBEXECDIR=\"$(libexecdir)\"					\
	-DLIBEXO_VERSION_API=\"$(LIBEXO_VERSION_API)\"

if HAVE_GIO_MODULEDIR
exomoduledir =								\
	$(GIOMODULEDIR)
else
exomoduledir =								\
	$(libdir)/gio/modules
endif

exomodule_LTLIBRARIES =							\
	libexo-module-1.la

libexo_module_1_la_SOURCES =						\
	exo-module.c

libexo_module_1_la_CFLAGS =						\
	$(GIO_CFLAGS)							\
	$(GIO_UNIX_CFLAGS) \
	$(GTK_CFLAGS)							\
	$(LIBXFCE4UTIL_CFLAGS)

libexo_module_1_la_LDFLAGS =						\
	-export_dynamic							\
	-avoid-version							\
	-module								\
	-no-undefined							\
	-export-symbols-regex '^g_io_module_(load|unload)'

libexo_module_1_la_DEPENDENCIES =					\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_API).la

libexo_module_1_la_LIBADD =						\
	$(GIO_LIBS)							\
	$(GIO_UNIX_LIBS) \
	$(GTK_LIBS)							\
	$(LIBXFCE4UTIL_LIBS)						\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_API).la

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
