# $Id$

INCLUDES = 								\
	-I$(top_builddir)						\
	-I$(top_srcdir)							\
	-DBINDIR=\"$(bindir)\"						\
	-DDATADIR=\"$(datadir)\"					\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	-DG_LOG_DOMAIN=\"exo-desktop-item-edit\"			\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

bin_PROGRAMS =								\
	exo-desktop-item-edit

exo_desktop_item_edit_SOURCES =						\
	exo-die-command-entry.c						\
	exo-die-command-entry.h						\
	exo-die-command-model.c						\
	exo-die-command-model.h						\
	exo-die-desktop-model.c						\
	exo-die-desktop-model.h						\
	exo-die-editor.c						\
	exo-die-editor.h						\
	exo-die-enum-types.c						\
	exo-die-enum-types.h						\
	exo-die-utils.c							\
	exo-die-utils.h							\
	main.c

exo_desktop_item_edit_CFLAGS =						\
	$(GTK_CFLAGS)							\
	$(GTHREAD_CFLAGS)						\
	$(LIBXFCE4UTIL_CFLAGS)

exo_desktop_item_edit_LDFLAGS =						\
	-no-undefined

exo_desktop_item_edit_DEPENDENCIES =					\
	$(top_builddir)/exo-support/libexo-support.la			\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

exo_desktop_item_edit_LDADD =						\
	$(GTK_LIBS)							\
	$(GTHREAD_LIBS)							\
	$(LIBXFCE4UTIL_LIBS)						\
	$(top_builddir)/exo-support/libexo-support.la			\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
