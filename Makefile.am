SUBDIRS =								\
	exo								\
	exo-csource							\
	exo-desktop-item-edit						\
	exo-helper							\
	exo-open							\
	docs								\
	icons								\
	pixmaps								\
	po								\
	tests

distclean-local:
	rm -rf *.cache *~

distuninstallcheck_listfiles = 						\ 
	find . -type f -print | grep -v ./share/icons/hicolor/icon-theme.cache

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
	intltool-extract.in						\
	intltool-merge.in						\
	intltool-update.in

DISTCLEANFILES =							\
	intltool-extract						\
	intltool-merge							\
	intltool-update

DISTCHECK_CONFIGURE_FLAGS =						\
	--enable-gtk-doc						\
	--enable-gen-doc						\
	--with-gio-module-dir=distcheck

# vi:set ts=8 sw=8 noet ai nocindent:
