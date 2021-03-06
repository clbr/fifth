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

#ifndef BLOCKING_H
#define BLOCKING_H

#include <urlmatch.h>

void loadblocking();
void saveblocking();

int isblocked(const char *);
int isinlineblocked(const char *, const char *);

struct charpair {
	const char *one;
	const char *two;
};

#define WHITENAME "whitelist"
#define WHITENAMEBIN "whitelist.bin"
#define BLACKNAME "blacklist"
#define BLACKNAMEBIN "blacklist.bin"
#define INLINEBLACKNAME "inlineblacklist"
#define HISTIGNORENAME "histignore"
#define HISTIGNORENAMEBIN "histignore.bin"

#endif
