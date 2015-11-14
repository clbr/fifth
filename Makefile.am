SUBDIRS = src doc

ACLOCAL_AMFLAGS = -I m4

dist_pkgdata_DATA = BACKGROUND DESIGN icon64.png icon128.png icon256.png \
		logo.png WEBKITBUGS
dist_pkgdata_SCRIPTS = convert-opera-bookmarks.sh

EXTRA_DIST = fifth.desktop

bindist: BASE = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
bindist: BINFILES = src/fifth $(dist_pkgdata_DATA) $(EXTRA_DIST) $(dist_pkgdata_SCRIPTS)
bindist: all
	GLIBC=_`LANG=C locale --version | grep libc | cut -d\) -f2 | sed 's@^ @glibc-@'`; \
	name=$(BASE)$${GLIBC}_`uname -m`; \
	rm -rf $(BASE); \
	mkdir $(BASE); \
	cp $(BINFILES) $(BASE); \
	mkdir $(BASE)/doc; cp doc/* $(BASE)/doc; rm $(BASE)/doc/Makefile*; \
	cp /usr/lib/{libssl,libcrypto}*.so.1.* $(BASE); \
	cp /usr/lib/{libcurl,libpng12,libphysfs,libcares,librtmp}*.so.? $(BASE); \
	cp /usr/lib/libjpeg*.so.?? $(BASE); \
	tar --numeric-owner -cf - $(BASE) | pigz -9 > $${name}.tgz; \
	rm -rf $(BASE)

.PHONY: bindist

install-data-hook: iconsdir = $(DESTDIR)$(datadir)/icons/hicolor
install-data-hook:
	install -D -m644 icon64.png $(iconsdir)/64x64/apps/fifth.png
	install -D -m644 icon128.png $(iconsdir)/128x128/apps/fifth.png
	install -D -m644 icon256.png $(iconsdir)/256x256/apps/fifth.png
	install -D -m644 fifth.desktop $(DESTDIR)$(datadir)/applications/fifth.desktop
