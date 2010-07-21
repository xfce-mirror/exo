
if HAVE_PYTHON
PYTHON_SUBDIR=python
endif

SUBDIRS =								\
	exo								\
	exo-csource							\
	exo-support							\
	exo-desktop-item-edit						\
	exo-gio-module							\
	exo-helper							\
	exo-open							\
	docs								\
	icons								\
	pixmaps								\
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

.PHONY: ChangeLog

ChangeLog: Makefile
	(GIT_DIR=$(top_srcdir)/.git git log > .changelog.tmp \
	&& mv .changelog.tmp ChangeLog; rm -f .changelog.tmp) \
	|| (touch ChangeLog; echo 'Git directory not found: installing possibly empty changelog.' >&2)

dist-hook: ChangeLog

EXTRA_DIST =								\
	AUTHORS								\
	COPYING								\
	COPYING.LIB							\
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
	--enable-gen-doc						\
	--with-gio-module-dir=distcheck

# vi:set ts=8 sw=8 noet ai nocindent:
