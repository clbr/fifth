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

#include "settings.h"

// Must be in alphabetical order.
const struct setting defaultSettings[] = {
	{"general.download-dir", ST_CHAR, { .c = "/tmp" }},
	{"general.homepage", ST_CHAR, { .c = "about:blank"}},
	{"general.javascript", ST_U32, { .u = 1}},
	{"window.h", ST_U32, { .u = 600}},
	{"window.w", ST_U32, { .u = 800}},
	{"window.x", ST_U32, { .u = 0}},
	{"window.y", ST_U32, { .u = 0}},
};

const u32 numDefaults = sizeof(defaultSettings) / sizeof(struct setting);
