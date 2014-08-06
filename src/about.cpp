/*
Copyright (C) 2014 Lauri Kasanen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "version.h"

#define xstr(s) str(s)
#define str(s) #s

static const char *aboutme() {

	const char str[] = "<html><body><center>"
				"<h3>Fifth "
#if GIT
				GITVERSION
#else
				VERSION
#endif
				", built on " __DATE__
				"<p>"
				"Running on WebkitFLTK " xstr(WK_FLTK_MAJOR) "."
				xstr(WK_FLTK_MINOR) "." xstr(WK_FLTK_PATCH)
				"</h3>"
				"</center></body></html>";

	return strdup(str);
}

static const char *aboutconfig() {
	return NULL;
}

const char *aboutpage(const char * const page) {

	#define is(a) if (!strcmp(page, a))
	is ("fifth") {
		return aboutme();
	} else is ("config") {
		return aboutconfig();
	}
	#undef is

	return NULL;
}
