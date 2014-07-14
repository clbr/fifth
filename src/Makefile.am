bin_PROGRAMS = fifth

fifth_SOURCES = config.cpp defaults.c helpers.cpp main.cpp remote.cpp \
		settings.cpp window.cpp status.cpp view.cpp tabs.cpp urlbar.cpp \
		urlbutton.cpp textweb.cpp inputplace.cpp keys.cpp screencap.cpp \
		bookmarks.cpp

CLEANFILES = version.h

$(fifth_SOURCES): version.h

$(top_srcdir)/.git:

version.h: $(top_srcdir)/.git
	$(srcdir)/getver.sh

AM_CPPFLAGS=-DDATADIR=\"$(pkgdatadir)\"
