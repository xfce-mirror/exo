# $Id$

SUBDIRS =								\
	examples

defsdir = $(datadir)/pygtk/2.0/defs/exo-$(LIBEXO_VERSION_API)
defs_DATA = exo.defs

pyexodir = $(pythondir)
pyexo_PYTHON = pyexo.py

exomoduledir = $(pyexecdir)/exo-$(LIBEXO_VERSION_API)/exo
exomodule_PYTHON = __init__.py

exobindingsdir = $(pyexecdir)/exo-$(LIBEXO_VERSION_API)
exobindings_LTLIBRARIES = _exo.la

_exo_la_CFLAGS =							\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	-I$(top_srcdir)							\
	$(LIBXFCEGUI4_CFLAGS)						\
	$(PYGTK_CFLAGS)							\
	$(PYTHON_INCLUDES)

_exo_la_LDFLAGS =							\
	-avoid-version							\
	-export-symbols-regex init_exo					\
	-module								\
	-no-undefined

_exo_la_LIBADD =							\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_API).la		\
	$(LIBXFCEGUI4_LIBS)

_exo_la_SOURCES =							\
	exomodule.c

nodist__exo_la_SOURCES =						\
	exo.c

BUILT_SOURCES =								\
	exo.c

exo.c: $(defs_DATA) $(srcdir)/exo.override Makefile
.defs.c:
	(cd $(srcdir) 							\
	 && $(PYGTK_CODEGEN)						\
	    --register $(PYGTK_DEFSDIR)/pango-types.defs		\
	    --register $(PYGTK_DEFSDIR)/atk-types.defs			\
	    --register $(PYGTK_DEFSDIR)/gdk-types.defs			\
	    --register $(PYGTK_DEFSDIR)/gtk-types.defs			\
	    --override $*.override					\
	    --prefix $* $*.defs) > xgen-$*.c				\
	&& cp xgen-$*.c $*.c						\
	&& rm -f xgen-$*.c

EXTRA_DIST =								\
	$(defs_DATA)							\
	$(exomodule_DATA)						\
	$(pyexo_PYTHON)							\
        abicheck.sh                                                     \
	exo.override

CLEANFILES =								\
	actual-abi							\
	exo.c								\
	expected-abi

if HAVE_GNUC_VISIBILITY
TESTS =									\
	abicheck.sh
endif

# vi:set ts=8 sw=8 et ai nocindent syntax=automake:
