# $Id$

defaultsdir = $(sysconfdir)/xdg/xfce4
defaults_DATA = helpers.rc

desktopdir = $(datadir)/xfce4/helpers
desktop_in_files =							\
	Terminal.desktop.in						\
	balsa.desktop.in						\
	debian-sensible-browser.desktop.in				\
	debian-x-terminal-emulator.desktop.in				\
	epiphany.desktop.in						\
	eterm.desktop.in						\
	evolution.desktop.in						\
	firefox.desktop.in						\
	galeon.desktop.in						\
	gnome-terminal.desktop.in					\
	kmail.desktop.in						\
	konqueror.desktop.in						\
	mozilla-browser.desktop.in					\
	mozilla-mailer.desktop.in					\
	opera-browser.desktop.in					\
	opera-mailer.desktop.in						\
	thunderbird.desktop.in						\
	xterm.desktop.in
desktop_DATA =								\
	$(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

DISTCLEANFILES =							\
	$(desktop_DATA)

EXTRA_DIST =								\
	$(defaults_DATA)						\
	$(desktop_in_files)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
