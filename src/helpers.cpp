#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
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
