INCLUDES = 								\
	-I$(top_srcdir)							\
	-DDBUS_API_SUBJECT_TO_CHANGE					\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	-DEXO_COMPILATION						\
	-DG_LOG_DOMAIN=\"exo\"

lib_LTLIBRARIES = libexo-0.1.la

libexoincludedir = $(includedir)/exo-0.1/exo

libexoinclude_HEADERS =							\
	exo.h								\
	exo-config.h							\
	exo-dbus.h							\
	exo-ellipsized-label.h						\
	exo-file-watch.h						\
	exo-gdk-pixbuf-extensions.h					\
	exo-gtk-extensions.h						\
	exo-ice.h							\
	exo-icon-bar.h							\
	exo-icon-view.h							\
	exo-md5.h							\
	exo-pango-extensions.h						\
	exo-property-proxy.h						\
	exo-string.h							\
	exo-toolbars-editor.h						\
	exo-toolbars-editor-dialog.h					\
	exo-toolbars-model.h						\
	exo-toolbars-view.h						\
	exo-uri.h

libexo_0_1_la_SOURCES =							\
	$(libexo_HEADERS)						\
	exo-config.c							\
	exo-dbus.c							\
	exo-ellipsized-label.c						\
	exo-file-watch.c						\
	exo-gdk-pixbuf-extensions.c					\
	exo-gtk-extensions.c						\
	exo-ice.c							\
	exo-icon-bar.c							\
	exo-icon-view.c							\
	exo-marshal.c							\
	exo-marshal.h							\
	exo-md5.c							\
	exo-pango-extensions.c						\
	exo-property-proxy.c						\
	exo-string.c							\
	exo-toolbars-editor.c						\
	exo-toolbars-editor-dialog.c					\
	exo-toolbars-model.c						\
	exo-toolbars-private.c						\
	exo-toolbars-private.h						\
	exo-toolbars-view.c						\
	exo-uri.c

libexo_0_1_la_CFLAGS =							\
	@DBUS_CFLAGS@							\
	@LIBXFCEGUI4_CFLAGS@

libexo_0_1_la_LDFLAGS =							\
	-export-dynamic							\
	-version-info @LIBEXO_VERINFO@					\
	-export-symbols-regex "^[^_].*"					\
	-no-undefined

libexo_0_1_la_LIBADD =							\
	@DBUS_LIBS@							\
	@LIBXFCEGUI4_LIBS@

exo-marshal.h: $(srcdir)/exo-marshal.list
	glib-genmarshal --prefix=_exo_marshal --header \
		$(srcdir)/exo-marshal.list > exo-marshal.h

exo-marshal.c: $(srcdir)/exo-marshal.list
	glib-genmarshal --prefix=_exo_marshal --body \
		$(srcdir)/exo-marshal.list > exo-marshal.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = exo-0.1.pc

EXTRA_DIST = 								\
	$(pkgconfig_DATA)						\
	exo-config.h.in							\
	exo-marshal.list

