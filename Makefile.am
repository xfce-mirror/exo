if HAVE_PYTHON
PYTHON_SUBDIR=python
endif

SUBDIRS =								\
	exo								\
	exo-csource							\
	docs								\
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
	exo.spec

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

dist-bz2: dist
	zcat $(PACKAGE)-$(VERSION).tar.gz | \
	    bzip2 --best -c > $(PACKAGE)-$(VERSION).tar.bz2

distcheck-bz2: distcheck
	zcat $(PACKAGE)-$(VERSION).tar.gz | \
            bzip2 --best -c > $(PACKAGE)-$(VERSION).tar.bz2
					
# vi:set ts=8 sw=8 noet ai nocindent:
