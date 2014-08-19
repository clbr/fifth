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

#define HISTFILE "history"

histbuf::histbuf(u32 max) {
	first = 0;
	used = 0;
	buf = (entry *) xcalloc(max, sizeof(entry));
	allocated = max;
}

histbuf::~histbuf() {
	clear();
	free(buf);
}

void histbuf::clear() {

	u32 i;
	for (i = 0; i < used; i++) {
		entry * const e = &buf[idx(i)];
		if (e->alloc)
			free(e->val.ptr);
	}

	first = 0;
	used = 0;
}

u32 histbuf::size() const {
	return used;
}

const char *histbuf::getURL(u32 i) const {
	if (i >= used)
		return NULL;

	const entry * const e = &buf[idx(i)];
	if (e->alloc)
		return e->val.ptr;
	else
		return e->val.c;
}

time_t histbuf::getTime(u32 i) const {
	if (i >= used)
		return 0;

	const entry * const e = &buf[idx(i)];
	return e->time;
}

void histbuf::add(const char *url, time_t time) {

	entry *e = NULL;

	if (used >= allocated) {
		e = &buf[idx(used)];
		// Bye first
		first++;
		first %= allocated;

		// May need to free the old ptr
		if (e->alloc)
			free(e->val.ptr);
	} else {
		e = &buf[idx(used)];
		used++;
	}

	e->time = time;
	if (strlen(url) + 1 > sizeof(e->val.c)) {
		e->alloc = 1;
		e->val.ptr = strdup(url);
	} else {
		e->alloc = 0;
		strcpy(e->val.c, url);
	}
}

u32 histbuf::idx(u32 i) const {
	return (first + i) % allocated;
}


void loadHistory() {
	const setting *s = getSetting("history.size");
	g->history = new histbuf(s->val.u);

	const int fd = openat(g->profilefd, HISTFILE, O_RDONLY);
	if (fd < 0)
		return;
	FILE *f = fdopen(fd, "r");
	if (!f)
		die("fdopen\n");

	const u32 len = 512;
	char buf[len];
	while (fgets(buf, len, f)) {
		nukenewline(buf);

		const time_t time = atoll(buf);
		const char *url = strchr(buf, ' ');
		if (!url)
			continue;
		url++;

		g->history->add(url, time);
	}

	fclose(f);
}

void saveHistory() {
	const int fd = openat(g->profilefd, HISTFILE, O_WRONLY | O_TRUNC | O_CREAT, 0600);
	if (fd < 0)
		return;
	FILE *f = fdopen(fd, "w");
	if (!f)
		die("fdopen\n");

	u32 i;
	const u32 max = g->history->size();
	for (i = 0; i < max; i++) {
		const time_t time = g->history->getTime(i);
		const char * const url = g->history->getURL(i);

		fprintf(f, "%lu %s\n", (unsigned long) time, url);
	}

	fclose(f);
}
