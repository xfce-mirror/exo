SUBDIRS =								\
	exo								\
	docs								\
	po

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
	TODO								\
	exo.spec.in							\
	exo.spec

ACLOCAL_AMFLAGS = -I m4
