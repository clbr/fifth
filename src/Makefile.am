bin_PROGRAMS = fifth

fifth_SOURCES = config.cpp defaults.c helpers.cpp main.cpp remote.cpp \
		settings.cpp window.cpp status.cpp view.cpp tabs.cpp urlbar.cpp \
		urlbutton.cpp textweb.cpp inputplace.cpp keys.cpp screencap.cpp \
		bookmarks.cpp adddial.cpp ssl.cpp blocking.cpp about.cpp crash.cpp \
		history.cpp zoomer.cpp error.cpp fl_browser_input.cpp viewbookmark.cpp \
		viewdl.cpp persite.cpp settingwindow.cpp uas.cpp \
		about.h blocking.h bookmarks.h config.h fl_browser_input.h gettext.h \
		helpers.h history.h inputplace.h keys.h lockicon.h lrtypes.h macros.h main.h \
		persite.h remote.h searchicons.h settings.h ssl.h status.h tabs.h textweb.h \
		uas.h urlbar.h urlbutton.h urlicons.h view.h window.h wmicon.h zoomer.h \
		Fl_Input_Choice.H

CLEANFILES = version.h

EXTRA_DIST = getver.sh checksettings.sh

$(fifth_SOURCES): version.h

$(top_srcdir)/.git:

version.h: $(top_srcdir)/.git
	$(srcdir)/getver.sh

AM_CPPFLAGS=-DDATADIR=\"$(pkgdatadir)\"
