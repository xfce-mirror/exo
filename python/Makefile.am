version = $(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR)

defsdir = $(datadir)/pygtk/2.0/defs
defs_DATA = exo.defs

exobindingsdir = $(pyexecdir)/exo-$(version)
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
	$(top_builddir)/exo/libexo-$(version).la			\
	$(LIBXFCEGUI4_LIBS)

_exo_la_SOURCES =							\
	exomodule.c

nodist__exo_la_SOURCES =						\
	exo.c

BUILT_SOURCES =								\
	exo.c

$(srcdir)/exo.c: $(defs_DATA) exo.override Makefile
.defs.c:
	(cd $(srcdir) 							\
	 && $(PYGTK_CODEGEN)						\
	    --register $(PYGTK_DEFSDIR)/pango-types.defs		\
	    --register $(PYGTK_DEFSDIR)/atk-types.defs			\
	    --register $(PYGTK_DEFSDIR)/gdk-types.defs			\
	    --register $(PYGTK_DEFSDIR)/gtk-types.defs			\
	    --override $(*F).override					\
	    --prefix $(*F) $(*F).defs) > xgen-pygc			\
	&& cp xgen-pygc $*.c						\
	&& rm -f xgen-pygc

EXTRA_DIST =								\
	$(defs_DATA)

CLEANFILES =								\
	$(srcdir)/exo.c


