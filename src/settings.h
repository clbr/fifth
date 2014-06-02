#ifndef SETTINGS_H
#define SETTINGS_H

#include "helpers.h"

enum settingtype {
	ST_CHAR,
	ST_FLOAT,
	ST_U32
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

#endif
