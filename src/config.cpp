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

#define CONFIGFILE "config"

enum {
	bufsize = 640
};

void nukenewline(char *ptr) {

	for (; *ptr; ptr++) {
		if (*ptr == '\n') {
			*ptr = '\0';
			return;
		}
	}
}

int settingcmp(const void *p1, const void *p2) {

	const setting * const a = (setting *) p1;
	const setting * const b = (setting *) p2;

	return strcmp(a->name, b->name);
}

static settingtype char2type(const char in) {

	switch (in) {
		case 'c':
			return ST_CHAR;
		break;
		case 'u':
			return ST_U32;
		break;
		case 'f':
			return ST_FLOAT;
		break;
		default:
			die(_("Corrupt config file (char2type)\n"));
	}
}

static void setupSetting(setting &s, const char * const name,
			const char * const val) {
	strncpy(s.name, name, 80);
	s.name[79] = '\0';

	switch (s.type) {
		case ST_CHAR:
			s.val.c = strdup(val);
		break;
		case ST_U32:
			s.val.u = atoi(val);
		break;
		case ST_FLOAT:
			s.val.f = atof(val);
		break;
		default:
			die(_("Corrupt config file (setupSetting)\n"));
	}
}

static void parseLine(const char *line) {

	char name[bufsize];
	char val[bufsize];
	char type;
	setting s;

	if (!memcmp(line, "site", 4)) {
		// Per-site setting
		char site[bufsize];
		line += 5;
		if (sscanf(line, "%s %s %c", site, name, &type) != 3)
			die(_("Faulty config line '%s'\n"), line);

		while (!isspace(*line))
			line++;
		while (isspace(*line))
			line++;
		while (!isspace(*line))
			line++;
		while (isspace(*line))
			line++;
		while (!isspace(*line))
			line++;
		while (isspace(*line))
			line++;
		strncpy(val, line, bufsize);
		val[bufsize - 1] = '\0';

		map<string, setting> &persite = g->sitesettings[site];

		s.type = char2type(type);
		setupSetting(s, name, val);

		// Check the name is a valid setting
		setting *check = (setting *) bsearch(name, g->settings,
					numDefaults, sizeof(setting),
					settingcmp);

		if (check)
			persite[s.name] = s;
		else
			die(_("Tried to add nonexistent site config item %s\n"), name);
	} else {
		if (sscanf(line, "%s %c", name, &type) != 2)
			die(_("Faulty config line '%s'\n"), line);

		while (!isspace(*line))
			line++;
		while (isspace(*line))
			line++;
		while (!isspace(*line))
			line++;
		while (isspace(*line))
			line++;
		strncpy(val, line, bufsize);
		val[bufsize - 1] = '\0';

		s.type = char2type(type);
		setupSetting(s, name, val);

		setting *check = (setting *) bsearch(name, g->settings,
					numDefaults, sizeof(setting),
					settingcmp);

		if (!check) {
			die(_("Tried to add nonexistent config item %s\n"), name);
		}

		if (check->type != s.type)
			die(_("Mismatching type on item %s\n"), name);

		memcpy(&check->val, &s.val, sizeof(s.val));
	}
}

static void parseFile(FILE * const f) {

	char buf[bufsize];

	while (fgets(buf, bufsize, f)) {
		const u32 len = strlen(buf);
		if (len < 5)
			continue;

		char *ptr = buf;
		while (isspace(*ptr))
			ptr++;

		if (*ptr == '#')
			continue;

		nukenewline(ptr);

		parseLine(ptr);
	}
}

void loadConfig() {

	timeval old, now;
	if (g->bench)
		gettimeofday(&old, NULL);

	// Setup defaults
	g->settings = (setting *) xcalloc(1, sizeof(setting) * numDefaults);
	memcpy(g->settings, defaultSettings, sizeof(setting) * numDefaults);

	int fd = openat(g->profilefd, CONFIGFILE, O_RDONLY);
	if (fd < 0) {
		saveConfig();
		return;
	}

	FILE *f = fdopen(fd, "r");
	if (!f)
		die("fdopen\n");

	parseFile(f);

	fclose(f);

	if (g->bench) {
		gettimeofday(&now, NULL);
		printf("Loading the config took %u us\n",
			usecs(old, now));
	}
}

void saveConfig() {

	timeval old, now;
	if (g->bench)
		gettimeofday(&old, NULL);

	// Backups
	renameat(g->profilefd, CONFIGFILE ".bak2",
			g->profilefd, CONFIGFILE ".bak3");
	renameat(g->profilefd, CONFIGFILE ".bak",
			g->profilefd, CONFIGFILE ".bak2");
	renameat(g->profilefd, CONFIGFILE,
			g->profilefd, CONFIGFILE ".bak");

	int fd = openat(g->profilefd, CONFIGFILE, O_WRONLY | O_TRUNC | O_CREAT, 0600);
	if (fd < 0)
		die(_("Cannot open config file for saving\n"));

	FILE *f = fdopen(fd, "w");
	if (!f)
		die("fdopen\n");

	u32 i;
	for (i = 0; i < numDefaults; i++) {
		char desc;
		switch (g->settings[i].type) {
			case ST_CHAR:
				desc = 'c';
				if (g->settings[i].val.c != defaultSettings[i].val.c) {
					if (g->settings[i].val.c &&
						!strcmp(g->settings[i].val.c,
							defaultSettings[i].val.c))
						break;
					fprintf(f, "%s %c %s\n", g->settings[i].name, desc,
						g->settings[i].val.c);
				}
			break;
			case ST_FLOAT:
				desc = 'f';
				if (g->settings[i].val.f != defaultSettings[i].val.f)
					fprintf(f, "%s %c %f\n", g->settings[i].name, desc,
						g->settings[i].val.f);
			break;
			case ST_U32:
				desc = 'u';
				if (g->settings[i].val.u != defaultSettings[i].val.u)
					fprintf(f, "%s %c %u\n", g->settings[i].name, desc,
						g->settings[i].val.u);
			break;
			case ST_COUNT:
				die(_("Tried to save corrupt config\n"));
		}
	}

	// Per-site config
	map<string, map<string, setting> >::const_iterator it;
	for (it = g->sitesettings.begin(); it != g->sitesettings.end(); it++) {
		const string &site = it->first;
		const map<string, setting> &set = it->second;

		map<string, setting>::const_iterator perit;

		for (perit = set.begin(); perit != set.end(); perit++) {
			const setting &cur = perit->second;
			char desc;
			switch (cur.type) {
				case ST_CHAR:
					desc = 'c';
					fprintf(f, "site %s %s %c %s\n", site.c_str(),
						cur.name, desc,
						cur.val.c);
				break;
				case ST_FLOAT:
					desc = 'f';
					fprintf(f, "site %s %s %c %f\n", site.c_str(),
						cur.name, desc,
						cur.val.f);
				break;
				case ST_U32:
					desc = 'u';
					fprintf(f, "site %s %s %c %u\n", site.c_str(),
						cur.name, desc,
						cur.val.u);
				break;
				case ST_COUNT:
					die(_("Tried to save corrupt config\n"));
			}
		}
	}

	fclose(f);

	if (g->bench) {
		gettimeofday(&now, NULL);
		printf("Saving the config took %u us\n",
			usecs(old, now));
	}
}
