bin_PROGRAMS = fifth

fifth_SOURCES = config.cpp  defaults.c  helpers.cpp  main.cpp  remote.cpp  \
		settings.cpp window.cpp

CLEANFILES = version.h

$(fifth_SOURCES): version.h

$(top_srcdir)/.git:

version.h: $(top_srcdir)/.git
	$(srcdir)/getver.sh

AM_CPPFLAGS=-DDATADIR=\"$(pkgdatadir)\"
