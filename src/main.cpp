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
#include "version.h"
#include <sys/file.h>
#include <getopt.h>

globals *g;

#define LOCKFILE "lock"
#define CRASHFILE "crash"

static void findProfile(const bool found) {

	const char *exedir = PHYSFS_getBaseDir();

	string test = exedir;
	test += "icon64.png";

	if (access(test.c_str(), F_OK) == 0) {
		// Embedded dir mode
		g->profilefd = open(exedir, O_RDONLY);
		if (g->profilefd < 0)
			die(_("Failed to get embedded profile dir, %s\n"), exedir);
		g->datafd = g->profilefd;
	} else if (found) {
		// System install with custom profile location
		g->datafd = open(DATADIR, O_RDONLY);
		if (g->datafd < 0)
			die(_("Failed to get data dir, %s\n"), DATADIR);
	} else {
		// System install
		const char *homedir = PHYSFS_getUserDir();

		test = homedir;
		test += ".fifth";
		mkdir(test.c_str(), 0700);

		g->profilefd = open(test.c_str(), O_RDONLY);
		if (g->profilefd < 0)
			die(_("Failed to get profile dir, %s\n"), test.c_str());

		g->datafd = open(DATADIR, O_RDONLY);
		if (g->datafd < 0)
			die(_("Failed to get data dir, %s\n"), DATADIR);
	}
}

static void help(const char * const argv0) {
	printf(_("Fifth %s\n\n"
		"	-h --help		This help\n"
		"	-p --profile [dir]	Use dir as the profile location\n"
		"	-v --version		Query the version\n"
		"\n"
		"	You can also give instructions to existing instances:\n"
		"	%s			# opens a new tab\n"
		"	%s google.com	# opens google.com in a new tab\n\n"),
		VERSION, argv0, argv0);
}

int main(int argc, char **argv) {

	g = new globals;

	// Opts
	bool customprofile = false;
	const char shorts[] = "hp:v";
	const struct option opts[] = {
		{"help", 0, NULL, 'h'},
		{"profile", 1, NULL, 'p'},
		{"version", 0, NULL, 'v'},
		{NULL, 0, NULL, 0},
	};

	while (1) {
		int c = getopt_long(argc, argv, shorts, opts, NULL);
		if (c == -1)
			break;
		switch (c) {
			case 'h':
			default:
				help(argv[0]);
				return 0;
			case 'v':
				printf("Fifth %s\n", VERSION);
				return 0;
			break;
			case 'p':
				g->profilefd = open(optarg, O_RDONLY | O_DIRECTORY);
				if (g->profilefd < 0)
					die(_("Failed to use custom profile dir %s\n"),
						optarg);
				customprofile = true;
			break;
		}
	}

	// Where are we?
	PHYSFS_init(argv[0]);
	findProfile(customprofile);
	PHYSFS_deinit();

	// Is this a crash, a remote call, or a normal start?
	if (faccessat(g->profilefd, LOCKFILE, R_OK | W_OK, 0) == 0) {
		g->lockfd = openat(g->profilefd, LOCKFILE, O_WRONLY | O_NONBLOCK);
		if (faccessat(g->profilefd, CRASHFILE, R_OK, 0) == 0) {
			// Crash. TODO
			puts(_("Crash recovery"));
			if (g->lockfd >= 0) close(g->lockfd);
			unlinkat(g->profilefd, LOCKFILE, 0);
		} else if (g->lockfd < 0 || flock(g->lockfd, LOCK_EX | LOCK_NB) == 0) {
			err(_("Detected stale lock file, but no crash report?\n"));
			if (g->lockfd >= 0) close(g->lockfd);
			unlinkat(g->profilefd, LOCKFILE, 0);
		} else {
			// Remote call
			long cur = fcntl(g->lockfd, F_GETFL);
			cur = cur & ~(O_NONBLOCK);
			if (fcntl(g->lockfd, F_SETFL, cur) != 0)
				die(_("Failed to block on lock file\n"));

			if (argc > 1) {
				puts(_("Opening requested URLs in existing instance."));

				u32 i;
				for (i = 1; i < (u32) argc; i++) {
					sendRemote(RT_URL, strlen(argv[i]) + 1, argv[i]);
				}
			} else {
				puts(_("Opening a new tab in existing instance."));
				sendRemote(RT_URL, 0, NULL);
			}
			return 0;
		}
	}

	// Normal start
	if (mkfifoat(g->profilefd, LOCKFILE, 0700) != 0)
		die(_("Failed creating lock file\n"));
	g->lockfd = openat(g->profilefd, LOCKFILE, O_RDONLY | O_NONBLOCK);
	if (flock(g->lockfd, LOCK_EX | LOCK_NB))
		die(_("Failed to lock the lock file\n"));
	g->newremotes = 0;
	pthread_mutex_init(&g->remotemutex, NULL);
	pthread_create(&g->remotetid, NULL, listenRemote, NULL);

	// Mainloop
	u32 i = 0;
	while (i < 10) {
		if (g->newremotes) {
			pthread_mutex_lock(&g->remotemutex);
			g->newremotes = 0;
			u32 r = 0;
			const u32 max = g->remotes.size();
			for (; r < max; r++) {
				// TODO
				if (g->remotes[r].size) {
					printf(_("Opening remote URL %s\n"),
						g->remotes[r].data);
					free(g->remotes[r].data);
				} else {
					puts(_("Opening new tab, requested by remote"));
				}
			}
			g->remotes.clear();
			pthread_mutex_unlock(&g->remotemutex);
		}

		sleep(1);
		i++;
	}

	pthread_cancel(g->remotetid);
	pthread_join(g->remotetid, NULL);
	unlinkat(g->profilefd, LOCKFILE, 0);
	delete g;
	return 0;
}
