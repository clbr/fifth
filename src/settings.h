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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "helpers.h"

enum settingtype {
	ST_CHAR,
	ST_FLOAT,
	ST_U32,
	ST_COUNT
};

struct setting {
	const char name[80];
	settingtype type;
	union {
		char *c;
		float f;
		u32 u;
	} val;
};

setting *getSetting(const char name[], const char * const site);

extern const setting defaultSettings[];

#endif
