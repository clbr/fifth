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

#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include "lrtypes.h"

#define PRINTF_WARNINGS(a,b) __attribute__ ((format (printf, a, b)))

// helpers
void *xcalloc(size_t nmemb, size_t size);
void *xmalloc(size_t size);
void die(const char fmt[], ...) PRINTF_WARNINGS(1, 2) __attribute__ ((noreturn));
void err(const char fmt[], ...) PRINTF_WARNINGS(1, 2);
float clampf(float in, float low, float high);
float smootherstep(float e0, float e1, float x);
float mix(float x, float y, float a);
unsigned ispow(const unsigned in);
unsigned npow(unsigned in);
u32 usecs(const struct timeval old, const struct timeval now);

static inline u32 u32max(u32 a, u32 b) {
	if (a > b) return a;
	return b;
}

#undef PRINTF_WARNINGS

#endif
