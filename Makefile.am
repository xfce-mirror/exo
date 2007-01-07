# $Id$

if HAVE_LIBNOTIFY
EXO_MOUNT_NOTIFY_SUBDIR=exo-mount-notify
endif

if HAVE_PYTHON
PYTHON_SUBDIR=python
endif

SUBDIRS =								\
	exo								\
	exo-csource							\
	exo-support							\
	exo-desktop-item-edit						\
	exo-hal								\
	exo-helper							\
	exo-mount							\
	$(EXO_MOUNT_NOTIFY_SUBDIR)					\
	exo-open							\
	docs								\
	icons								\
	pixmaps								\
	po								\
	po-doc								\
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
	intltool-extract.in						\
	intltool-merge.in						\
	intltool-update.in

DISTCLEANFILES =							\
	exo.spec							\
	intltool-extract						\
	intltool-merge							\
	intltool-update

DISTCHECK_CONFIGURE_FLAGS =						\
	--enable-gtk-doc						\
	--enable-xml2po							\
	--enable-xsltproc

# vi:set ts=8 sw=8 noet ai nocindent:
