# $Id$

defaultsdir = $(sysconfdir)/xdg/xfce4
defaults_DATA = helpers.rc

desktopdir = $(datadir)/xfce4/helpers
desktop_in_in_files =							\
	Terminal.desktop.in.in						\
	balsa.desktop.in.in						\
	debian-sensible-browser.desktop.in.in				\
	debian-x-terminal-emulator.desktop.in.in			\
	epiphany.desktop.in.in						\
	eterm.desktop.in.in						\
	evolution.desktop.in.in						\
	firefox.desktop.in.in						\
	galeon.desktop.in.in						\
	gnome-terminal.desktop.in.in					\
	kmail.desktop.in.in						\
	konqueror.desktop.in.in						\
	mozilla-browser.desktop.in.in					\
	mozilla-mailer.desktop.in.in					\
	mutt.desktop.in.in						\
	opera-browser.desktop.in.in					\
	opera-mailer.desktop.in.in					\
	sylpheed.desktop.in.in						\
	sylpheed-claws.desktop.in.in					\
	thunderbird.desktop.in.in					\
	xterm.desktop.in.in
desktop_in_files = $(desktop_in_in_files:.desktop.in.in=.desktop.in)
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
%.desktop.in: %.desktop.in.in
	sed -e "s,\@libexecdir\@,$(libexecdir),g" < $< > $@
@INTLTOOL_DESKTOP_RULE@

libexec_SCRIPTS =							\
	exo-compose-mail-0.3

DISTCLEANFILES =							\
	$(desktop_in_files)						\
	$(desktop_DATA)

EXTRA_DIST =								\
	$(defaults_DATA)						\
	$(desktop_in_in_files)						\
	exo-compose-mail-0.3

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
