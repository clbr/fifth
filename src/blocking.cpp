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

#define ATOMIC_SET(ptr, val) __sync_bool_compare_and_swap(&ptr, ptr, val)

static void whitetext() {
	const int fd = openat(g->profilefd, WHITENAME, O_RDONLY);
	if (fd < 0)
		die("Whitelist exists but failed open?\n");
	ATOMIC_SET(g->whitelist, url_init_file2(fd));
}

static void blacktext() {
	const int fd = openat(g->profilefd, BLACKNAME, O_RDONLY);
	if (fd < 0)
		die("Blacklist exists but failed open?\n");
	ATOMIC_SET(g->blacklist, url_init_file2(fd));
}

void loadblocking() {

	timeval old, now;
	if (g->bench)
		gettimeofday(&old, NULL);

	ATOMIC_SET(g->blacklist, NULL);
	ATOMIC_SET(g->whitelist, NULL);

	const bool whitefound = !faccessat(g->profilefd, WHITENAME, R_OK, 0);
	const bool whitebinfound = !faccessat(g->profilefd, WHITENAMEBIN, R_OK, 0);
	const bool blackfound = !faccessat(g->profilefd, BLACKNAME, R_OK, 0);
	const bool blackbinfound = !faccessat(g->profilefd, BLACKNAMEBIN, R_OK, 0);

	struct stat textst, binst;
	if (whitefound) {
		if (whitebinfound) {
			// Check the binary is more recent - someone may have hand-edited
			if (fstatat(g->profilefd, WHITENAME, &textst, 0))
				die("Whitelist exists but failed stat?\n");
			if (fstatat(g->profilefd, WHITENAMEBIN, &binst, 0))
				die("Whitelist binary exists but failed stat?\n");

			if (textst.st_mtime > binst.st_mtime) {
				whitetext();
			} else {
				const int fd = openat(g->profilefd, WHITENAMEBIN, O_RDONLY);
				if (fd < 0)
					die("Whitelist binary exists but failed open?\n");
				ATOMIC_SET(g->whitelist, url_init_file2(fd));
			}
		} else {
			whitetext();
		}
	}

	if (blackfound) {
		if (blackbinfound) {
			// Check the binary is more recent - someone may have hand-edited
			if (fstatat(g->profilefd, BLACKNAME, &textst, 0))
				die("Blacklist exists but failed stat?\n");
			if (fstatat(g->profilefd, BLACKNAMEBIN, &binst, 0))
				die("Blacklist binary exists but failed stat?\n");

			if (textst.st_mtime > binst.st_mtime) {
				blacktext();
				saveblocking();
			} else {
				const int fd = openat(g->profilefd, BLACKNAMEBIN, O_RDONLY);
				if (fd < 0)
					die("Blacklist binary exists but failed open?\n");
				ATOMIC_SET(g->blacklist, url_init_file2(fd));
			}
		} else {
			blacktext();
			saveblocking();
		}
	}

	if (g->bench) {
		gettimeofday(&now, NULL);
		printf("Loading the blocklists took %u us\n",
			usecs(old, now));
	}
}

void saveblocking() {

	timeval old, now;
	if (g->bench)
		gettimeofday(&old, NULL);

	if (g->whitelist) {
		const int fd = openat(g->profilefd, WHITENAMEBIN, O_WRONLY | O_CREAT, 0644);
		if (fd < 0)
			die("Error saving the whitelist\n");
		if (url_save_optimized2(g->whitelist, fd))
			die("Error saving the whitelist\n");
	}

	if (g->blacklist) {
		const int fd = openat(g->profilefd, BLACKNAMEBIN, O_WRONLY | O_CREAT, 0644);
		if (fd < 0)
			die("Error saving the blacklist\n");
		if (url_save_optimized2(g->blacklist, fd))
			die("Error saving the blacklist\n");
	}

	if (g->bench) {
		gettimeofday(&now, NULL);
		printf("Saving the blocklists took %u us\n",
			usecs(old, now));
	}
}

// Return 0 for ok, 1 to block
int isblocked(const char *url) {

	static const char * const debug = getenv("FIFTH_DEBUG_BLOCKING");
	int ret;

	// These should be atomic reads, but on 64-bit it doesn't matter.
	if (g->whitelist) {
		ret = url_match(g->whitelist, url);
		if (!ret) {
			if (debug)
				err(_("Whitelist blocked '%s'\n"), url);
			return 1;
		}
	}

	if (g->blacklist) {
		ret = url_match(g->blacklist, url);
		if (ret) {
			if (debug)
				err(_("Blacklist blocked '%s'\n"), url);
			return 1;
		}
	}

	return 0;
}
