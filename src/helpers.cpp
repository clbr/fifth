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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "helpers.h"

void *xcalloc(size_t nmemb, size_t size) {

	void *tmp = calloc(nmemb, size);
	if (!tmp) die("Out of memory\n");

	return tmp;
}

void *xmalloc(size_t size) {

	void *tmp = malloc(size);
	if (!tmp) die("Out of memory\n");

	return tmp;
}

void die(const char fmt[], ...) {

	va_list ap;

	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);

	va_end(ap);
	exit(1);
}

void err(const char fmt[], ...) {

	va_list ap;

	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);

	va_end(ap);
}

float clampf(float in, float low, float high) {
	return in > high ? high : in < low ? low : in;
}

float smootherstep(float e0, float e1, float x) {
	x = clampf((x - e0)/(e1 - e0), 0, 1);

	return x*x*x*(x*(x*6 - 15) + 10);
}

float mix(float x, float y, float a) {
	return x * (1 - a) + y * a;
}

unsigned ispow(const unsigned in) {

	if (in < 2) return 0;

	return !(in & (in - 1));
}

unsigned npow(unsigned in) {

	if (ispow(in)) return in;

	in |= in >> 1;
	in |= in >> 2;
	in |= in >> 4;
	in |= in >> 8;
	in |= in >> 16;

	return in + 1;
}

u32 usecs(const timeval old, const timeval now) {

	u32 us = (now.tv_sec - old.tv_sec) * 1000 * 1000;
	us += now.tv_usec - old.tv_usec;

	return us;
}

u64 msec() {
	struct timeval t;
	gettimeofday(&t, NULL);

	u64 ms = t.tv_sec * 1000;
	ms += t.tv_usec / 1000;
	return ms;
}

int allspace(const char *in) {
	for (; *in; in++) {
		if (!isspace(*in))
			return 0;
	}

	return 1;
}

ssize_t sread(const int fd, void *buf, const size_t count) {

	u32 read = 0;
	u8 *pos = (u8 *) buf;
	while (read < count) {
		const int ret = ::read(fd, pos, count);

		if (ret < 0)
			return ret;
		if (ret == 0)
			return read;

		pos += ret;
		read += ret;
	}

	return read;
}

ssize_t swrite(const int fd, const void *buf, const size_t count) {

	u32 written = 0;
	const u8 *pos = (const u8 *) buf;

	while (written != count) {
		const int ret = write(fd, pos, count);

		if (ret <= 0)
			return ret;

		pos += ret;
		written += ret;
	}

	return written;
}
