#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "lrtypes.h"

#define PRINTF_WARNINGS(a,b) __attribute__ ((format (printf, a, b)))

// helpers
void *xcalloc(size_t nmemb, size_t size);
void *xmalloc(size_t size);
void die(const char fmt[], ...) PRINTF_WARNINGS(1, 2) __attribute__ ((noreturn));
float clampf(float in, float low, float high);
float smootherstep(float e0, float e1, float x);
float mix(float x, float y, float a);
unsigned ispow(const unsigned in);
unsigned npow(unsigned in);

static inline u32 u32max(u32 a, u32 b) {
	if (a > b) return a;
	return b;
}

#undef PRINTF_WARNINGS

#endif
