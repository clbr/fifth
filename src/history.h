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

#ifndef HISTORY_H
#define HISTORY_H

#include <time.h>
#include "main.h"

class histbuf {
public:
	histbuf(u32 max);
	~histbuf();

	void clear();

	u32 size() const;

	const char *getURL(u32) const;
	time_t getTime(u32) const;

	void add(const char *, time_t);
private:
	struct entry {
		union {
			char c[80];
			char *ptr;
		} val;

		time_t time;
		bool alloc;
	};

	u32 idx(u32 i) const;

	entry *buf;
	u32 first;
	u32 used;
	u32 allocated;
};

void loadHistory();
void saveHistory();

#endif
