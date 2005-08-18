# $Id$

SUBDIRS =								\
	data

INCLUDES =								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"exo-tests\"

TESTS =									\
	test-exo-csource						\
	test-exo-object							\
	test-exo-noop

check_PROGRAMS =							\
	test-exo-csource						\
	test-exo-object							\
	test-exo-noop

test_exo_csource_SOURCES =						\
	test-exo-csource.c						\
	test-exo-csource-data.c

test_exo_csource_CFLAGS =						\
	$(GLIB_CFLAGS)

test_exo_csource_LDADD =						\
	$(GLIB_LIBS)

test_exo_noop_SOURCES =							\
	test-exo-noop.c

test_exo_noop_CFLAGS =							\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	$(GTK_CFLAGS)							\
	$(LIBXFCE4UTIL_CFLAGS)

test_exo_noop_DEPENDENCIES =						\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

test_exo_noop_LDADD =							\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

test_exo_object_SOURCES =						\
	test-exo-object.c

test_exo_object_CFLAGS =						\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	$(GTK_CFLAGS)							\
	$(LIBXFCE4UTIL_CFLAGS)

test_exo_object_DEPENDENCIES =						\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

test_exo_object_LDADD =							\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

DISTCLEANFILES = test-exo-csource-data.c
BUILT_SOURCES = test-exo-csource-data.c

test-exo-csource-data.c: Makefile $(srcdir)/data/data.bin $(srcdir)/data/data.txt
	$(top_builddir)/exo-csource/exo-csource --extern --build-list data_bin $(srcdir)/data/data.bin data_txt $(srcdir)/data/data.txt > test-exo-csource-data.c

clean-local:
	rm -f *.core core core.*

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
