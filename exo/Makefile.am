INCLUDES = 								\
	-I$(top_srcdir)							\
	-DDBUS_API_SUBJECT_TO_CHANGE					\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	-DEXO_COMPILATION						\
	-DG_LOG_DOMAIN=\"exo\"						\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

lib_LTLIBRARIES = libexo-0.2.la

libexo_headers =							\
	exo-pango-extensions.h

libexoincludedir = $(includedir)/exo-0.2/exo

libexoinclude_HEADERS =							\
	$(libexo_headers)						\
	exo.h								\
	exo-binding.h							\
	exo-cell-renderer-ellipsized-text.h				\
	exo-config.h							\
	exo-dbus.h							\
	exo-ellipsized-label.h						\
	exo-enum-types.h						\
	exo-file-watch.h						\
	exo-gdk-pixbuf-extensions.h					\
	exo-gtk-extensions.h						\
	exo-gobject-extensions.h					\
	exo-ice.h							\
	exo-icon-bar.h							\
	exo-icon-view.h							\
	exo-md5.h							\
	exo-property-proxy.h						\
	exo-string.h							\
	exo-toolbars-editor.h						\
	exo-toolbars-editor-dialog.h					\
	exo-toolbars-model.h						\
	exo-toolbars-view.h						\
	exo-uri.h							\
	exo-xsession-client.h

libexo_0_2_la_SOURCES =							\
	$(libexo_HEADERS)						\
	exo-binding.c							\
	exo-cell-renderer-ellipsized-text.c				\
	exo-config.c							\
	exo-dbus.c							\
	exo-ellipsized-label.c						\
	exo-enum-types.c						\
	exo-file-watch.c						\
	exo-gdk-pixbuf-extensions.c					\
	exo-gtk-extensions.c						\
	exo-gobject-extensions.c					\
	exo-ice.c							\
	exo-icon-bar.c							\
	exo-icon-view.c							\
	exo-marshal.c							\
	exo-marshal.h							\
	exo-md5.c							\
	exo-pango-extensions.c						\
	exo-private.c							\
	exo-private.h							\
	exo-property-proxy.c						\
	exo-string.c							\
	exo-toolbars-editor.c						\
	exo-toolbars-editor-dialog.c					\
	exo-toolbars-model.c						\
	exo-toolbars-private.c						\
	exo-toolbars-private.h						\
	exo-toolbars-view.c						\
	exo-uri.c							\
	exo-xsession-client.c

libexo_0_2_la_CFLAGS =							\
	@DBUS_CFLAGS@							\
	@LIBSM_CFLAGS@							\
	@LIBX11_CFLAGS@							\
	@LIBXFCEGUI4_CFLAGS@

libexo_0_2_la_LDFLAGS =							\
	-export-dynamic							\
	-version-info @LIBEXO_VERINFO@					\
	-export-symbols-regex "^[^_].*"					\
	-no-undefined

libexo_0_2_la_LIBADD =							\
	@DBUS_LIBS@							\
	@LIBSM_LIBS@							\
	@LIBX11_LIBS@							\
	@LIBXFCEGUI4_LIBS@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = exo-0.2.pc

regenerate:
	( cd $(srcdir) && glib-mkenums \
		--fhead "#ifndef __EXO_ENUM_TYPES_H__\n#define __EXO_ENUM_TYPES_H__\n#include <libxfce4util/libxfce4util.h>\nG_BEGIN_DECLS\n" \
		--fprod "/* enumerations from \"@filename@\" */\n" \
		--vhead "GType @enum_name@_get_type (void);\n#define EXO_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
		--ftail "G_END_DECLS\n\n#endif /* !__EXO_ENUM_TYPES_H__ */" \
		$(libexo_headers) ) > xgen \
	&& (cmp -s xgen exo-enum-types.h || cp xgen exo-enum-types.h ) \
	&& rm -f xgen
	( cd $(srcdir) && glib-mkenums \
		--fhead "#undef GTK_DISABLE_DEPRECATED\n#define GTK_ENABLE_BROKEN\n#include <exo/exo.h>\n" \
		--fprod "\n/* enumerations from \"@filename@\" */" \
		--vhead "GType\n@enum_name@_get_type (void)\n{\n\tstatic GType type = 0;\n\tif (type == 0) {\n\tstatic const G@Type@Value values[] = {"\
		--vprod "\t{ @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
		--vtail "\t{ 0, NULL, NULL }\n\t};\n\ttype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n\treturn type;\n}\n" \
		$(libexo_headers) ) > xgen \
	&& (cmp -s xgen exo-enum-types.c || cp xgen exo-enum-types.c ) \
	&& rm -f xgen
	glib-genmarshal --prefix=_exo_marshal --header \
		$(srcdir)/exo-marshal.list > exo-marshal.h
	glib-genmarshal --prefix=_exo_marshal --body \
		$(srcdir)/exo-marshal.list > exo-marshal.c
	echo "Regenerated source files"

# required for gtk-doc
dist-hook: all

EXTRA_DIST = 								\
	exo-marshal.list

