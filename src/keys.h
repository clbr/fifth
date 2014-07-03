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

#ifndef KEYS_H
#define KEYS_H

#include "main.h"

enum {
	KEYBIT_CTRL = 1 << 25,
	KEYBIT_ALT = 1 << 26,
	KEYBIT_SHIFT = 1 << 27,
};

struct keybinding {
	u32 key;
	bool ctrl;
	bool alt;
	bool shift;

	bool operator < (const keybinding &other) const {
		if (key != other.key)
			return key < other.key;
		if (ctrl != other.ctrl)
			return ctrl < other.ctrl;
		if (alt != other.alt)
			return alt < other.alt;
		if (shift != other.shift)
			return shift < other.shift;

		return false;
	}
};

typedef void (*keyfunc)();

void loadkeys();
u32 keytou32(const keybinding &k);
keybinding u32tokey(const u32 in);
u32 menukey(const char *);

void screencap();

#endif
