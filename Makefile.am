if HAVE_PYTHON
PYTHON_SUBDIR=python
endif

SUBDIRS =								\
	exo								\
	docs								\
	po								\
	$(PYTHON_SUBDIR)

distclean-local:
	rm -rf *.spec *.cache *~

dist-bz2: dist
	zcat $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz | \
	  bzip2 --best -c > $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.bz2

distcheck-bz2: distcheck dist-bz2

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

# vi:set ts=8 sw=8 noet ai nocindent:
