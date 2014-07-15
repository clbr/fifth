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

#include "main.h"

setting *getSetting(const char name[], const char * const site) {

	// Get a setting, possibly overrided at the site level
	if (site && g->sitesettings.count(site)) {
		vector<setting> &vec = g->sitesettings[site];
		const u32 max = vec.size();
		u32 i;
		for (i = 0; i < max; i++) {
			if (!strcmp(name, vec[i].name))
				return &vec[i];
		}
	}

	return (setting *) bsearch(name, g->settings, numDefaults,
					sizeof(setting), settingcmp);
}

void url2site(const char url[], char site[], const u32 size) {

	const char *start = strstr(url, "://");
	if (start)
		start += 3;
	else
		start = url;

	if (!memcmp(start, "www.", 4))
		start += 4;

	u32 pos;
	for (pos = 0; pos < size; pos++) {
		if (start[pos] == '/')
			break;
		site[pos] = start[pos];
	}

	site[pos] = '\0';
}
