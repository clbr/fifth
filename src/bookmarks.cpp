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

#define BOOKMARKFILE "bookmarks"

void loadbookmarks() {

	const int fd = openat(g->profilefd, BOOKMARKFILE, O_RDONLY);
	if (fd < 0)
		return;
	FILE * const f = fdopen(fd, "r");
	if (!f)
		return;

	enum {
		BUFSIZE = 640
	};

	char tmp[BUFSIZE];
	bookmark mark = { NULL, NULL };
	while (fgets(tmp, BUFSIZE, f)) {
		nukenewline(tmp);

		if (strlen(tmp) < 2)
			continue;

		if (!strncmp(tmp, "name", 4)) {
			if (mark.name || mark.url)
				die("Corrupted bookmark file, line '%s'\n", tmp);

			char *ptr = tmp + 5;
			mark.name = NULL;
			if (strlen(ptr) > 1)
				mark.name = strdup(ptr);
		} else if (!strncmp(tmp, "url", 3)) {
			char *ptr = tmp + 4;
			mark.url = NULL;
			if (strlen(ptr) > 1)
				mark.url = strdup(ptr);

			g->bookmarks.push_back(mark);

			mark.name = NULL;
			mark.url = NULL;
		} else {
			die("Unknown bookmark line '%s'\n", tmp);
		}
	}

	fclose(f);
}

void savebookmarks() {

	const int fd = openat(g->profilefd, BOOKMARKFILE, O_WRONLY);
	if (fd < 0)
		die("Failed saving bookmarks\n");
	FILE * const f = fdopen(fd, "w");
	if (!f)
		die("Failed saving bookmarks\n");

	fclose(f);

}
