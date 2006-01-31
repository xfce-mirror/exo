# $Id$

if HAVE_PYTHON
PYTHON_SUBDIR=python
endif

SUBDIRS =								\
	exo								\
	exo-csource							\
	exo-helper							\
	exo-open							\
	docs								\
	icons								\
	po								\
	$(PYTHON_SUBDIR)						\
	tests

AUTOMAKE_OPTIONS =							\
	1.8								\
	dist-bzip2

distclean-local:
	rm -rf *.spec *.cache *~

rpm: dist
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
	@rm -f $(PACKAGE)-$(VERSION).tar.gz

EXTRA_DIST =								\
	AUTHORS								\
	COPYING								\
	ChangeLog							\
	HACKING								\
	INSTALL								\
	NEWS								\
	README								\
	THANKS								\
	TODO								\
	exo.spec.in							\
	exo.spec							\
	intltool-extract.in						\
	intltool-merge.in						\
	intltool-update.in

DISTCLEANFILES =							\
	intltool-extract						\
	intltool-merge							\
	intltool-update

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

# vi:set ts=8 sw=8 noet ai nocindent:
