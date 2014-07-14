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
#include <FL/Fl_Pack.H>

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
		test += "." PACKAGE;
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
	printf(_("\n" PACKAGE_NAME " %s\n"
		"Using WebkitFLTK %u.%u.%u\n\n"
		"	-b --bench		Benchmark mode\n"
		"	-k --debug-keys		Print info on unrecognized shortcut keys\n"
		"	-h --help		This help\n"
		"	-p --profile [dir]	Use dir as the profile location\n"
		"	-v --version		Query the version\n"
		"\n"
		"	You can also give instructions to existing instances:\n"
		"	%s			# opens a new tab\n"
		"	%s google.com	# opens google.com in a new tab\n\n"),
#if GIT
		GITVERSION,
#else
		VERSION,
#endif
		WK_FLTK_MAJOR,
		WK_FLTK_MINOR,
		WK_FLTK_PATCH,
		argv0, argv0);
}

static void quitcb(Fl_Widget *, void *) {
	g->w->hide();
}

static void newtabcb(Fl_Widget *, void *) {
	newtab();
}

static void closetabcb(Fl_Widget *, void *) {
	closetab();
}

static void screencapcb(Fl_Widget *, void *) {
	screencap();
}

static void bugcb(Fl_Widget *, void *) {
	newtab("http://github.com/clbr/fifth/issues");
}

static void bookmarkcb(Fl_Widget *, void *ptr) {
	const char *url = (const char *) ptr;
	g->tabs[g->curtab].state = TS_WEB;
	g->tabs[g->curtab].web->load(url);
	g->tabs[g->curtab].web->take_focus();
}

static void addbookmarkcb(Fl_Widget *, void *) {
	addbookmark();
}

void generatemenu() {

	g->menu->clear();

	g->menu->add(_("&File/&New tab"), menukey("keys.newtab"), newtabcb);
	g->menu->add(_("&File/&Close tab"), menukey("keys.closetab"), closetabcb, 0,
					FL_MENU_DIVIDER);
	g->menu->add(_("&File/&Save screencap"), menukey("keys.screencap"), screencapcb, 0,
					FL_MENU_DIVIDER);
	g->menu->add(_("&File/&Quit"), menukey("keys.quit"), quitcb);

	g->menu->add(_("&Edit/&Undo"), 0, 0, 0, FL_MENU_INACTIVE);
	g->menu->add(_("&Edit/&Redo"), 0, 0, 0, FL_MENU_INACTIVE | FL_MENU_DIVIDER);
	g->menu->add(_("&Edit/&Cut"), 0, 0, 0, FL_MENU_INACTIVE);
	g->menu->add(_("&Edit/&Copy"), 0, 0, 0, FL_MENU_INACTIVE);
	g->menu->add(_("&Edit/&Paste"), 0, 0, 0, FL_MENU_INACTIVE | FL_MENU_DIVIDER);
	g->menu->add(_("&Edit/&Select all"), 0, 0, 0, FL_MENU_INACTIVE);
	g->menu->add(_("&Edit/&Find"), 0, 0, 0, FL_MENU_INACTIVE);

	g->menu->add(_("&Bookmarks/&Add bookmark"), menukey("keys.addbookmark"), addbookmarkcb);
	g->menu->add(_("&Bookmarks/&Edit bookmarks"), 0, 0, 0, FL_MENU_INACTIVE | FL_MENU_DIVIDER);

	u32 i;
	const u32 max = g->bookmarks.size();
	const string base = _("&Bookmarks/");
	u32 depth = 0;
	string dirs[32];
	for (i = 0; i < max; i++) {
		const bookmark &cur = g->bookmarks[i];
		if (!cur.name) {
			depth--;
		} else if (!cur.url) {
			depth++;
			if (depth >= 32)
				die("Too deep bookmark hierarchy\n");
			dirs[depth] = cur.name;
		} else {
			string todo = base;
			u32 d;
			for (d = 1; d <= depth; d++) {
				todo += dirs[d];
				todo += "/";
			}
			todo += cur.name;

			g->menu->add(todo.c_str(), 0, bookmarkcb, g->bookmarks[i].url);
		}
	}
	if (depth != 0)
		die("Bookmark corruption\n");

	g->menu->add(_("&Tools/&Web inspector"), 0, 0, 0, FL_MENU_INACTIVE);
	g->menu->add(_("&Tools/&Error console"), 0, 0, 0, FL_MENU_INACTIVE | FL_MENU_DIVIDER);
	g->menu->add(_("&Tools/&Page settings"), 0, 0, 0, FL_MENU_INACTIVE);
	g->menu->add(_("&Tools/&Settings"), 0, 0, 0, FL_MENU_INACTIVE);

	g->menu->add(_("&Help/&Report a bug"), 0, bugcb);
	g->menu->add(_("&Help/&About"), 0, 0, 0, FL_MENU_INACTIVE);
}

int main(int argc, char **argv) {

	g = new globals;
	g->bench = false;
	g->debugkeys = false;

	srand(time(NULL));
	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC, "C");

	// Opts
	bool customprofile = false;
	const char shorts[] = "bkhp:v";
	const struct option opts[] = {
		{"bench", 0, NULL, 'b'},
		{"debug-keys", 0, NULL, 'k'},
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
#if GIT
				printf(PACKAGE " %s\n", GITVERSION);
#else
				printf(PACKAGE " %s\n", VERSION);
#endif
				return 0;
			break;
			case 'p':
				g->profilefd = open(optarg, O_RDONLY | O_DIRECTORY);
				if (g->profilefd < 0)
					die(_("Failed to use custom profile dir %s\n"),
						optarg);
				customprofile = true;
			break;
			case 'b':
				g->bench = true;
			break;
			case 'k':
				g->debugkeys = true;
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

	loadConfig();
	loadkeys();
	loadbookmarks();

	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 1024*512);
	pthread_create(&tid, &attr, listenRemote, NULL);
	pthread_attr_destroy(&attr);

	if (argc > 1) {
		u32 i;
		pthread_mutex_lock(&g->remotemutex);
		for (i = optind; i < (u32) argc; i++) {
			remotemsg m;
			m.type = RT_URL;
			m.size = strlen(argv[i]);
			m.data = strdup(argv[i]);
			g->remotes.push_back(m);
			g->newremotes = 1;
		}
		pthread_mutex_unlock(&g->remotemutex);
	}

	webkitInit();

	u32 x, y, w, h;
	setting *s = getSetting("window.x", NULL);
	x = s->val.u;
	s = getSetting("window.y", NULL);
	y = s->val.u;
	s = getSetting("window.w", NULL);
	w = s->val.u;
	s = getSetting("window.h", NULL);
	h = s->val.u;

	s = getSetting("fltk.scheme", NULL);
	Fl::scheme(s->val.c);

	g->w = new window(x, y, w, h);
	g->w->size_range(640, 480, 5120, 5120);

	Fl_Pack *pack = new Fl_Pack(0, 0, w, h);
	pack->type(Fl_Pack::VERTICAL);
	g->w->resizable(pack);

	const u32 menuheight = 22;
	const u32 tabheight = 28;
	const u32 urlheight = 32;
	const u32 statusheight = 22;

	// Menu
	Fl_Menu_Bar *menu = new Fl_Menu_Bar(0, 0, w, menuheight);
	menu->textsize(12);
	g->menu = menu;
	generatemenu();

	// Ordering and presence of the middle widgets
	s = getSetting("window.bars", NULL);

	u32 num = 1;
	const char *ptr = s->val.c;
	for (; *ptr; ptr++)
		if (*ptr == ',')
			num++;

	u32 i;
	ptr = s->val.c;
	for (i = 0; i < num; i++) {
		// Get the ith word
		const char *end = strchrnul(ptr, ',');
		const u32 len = end - ptr;

		#define entry(a) if (!strncmp(a, ptr, len))

		entry("tab") {
			g->tabwidget = new tabbar(0, 0, w, tabheight);
		} else entry("url") {
			g->url = new urlbar(0, 0, w, urlheight);
		} else entry("main") {
			view *v = new view(0, 0, 10,
				h - menuheight - tabheight - urlheight - statusheight);
			pack->resizable(v);
			g->v = v;
		} else {
			printf("Unknown window.bars entry '%.*s'\n",
				len, ptr);
		}

		#undef entry

		ptr = end + 1;
	}

	new statusbar(0, 0, w, statusheight);

	pack->end();

	g->w->end();
	g->w->label("Fifth");
	g->w->show();

	// What to do on startup? If an url was given, don't do the normal start
	if (optind >= argc) {
		s = getSetting("general.startup", NULL);
		switch ((startup) s->val.u) {
			case START_DIAL:
				newtab();
			break;
			case START_HOME:
				s = getSetting("general.homepage", NULL);
				newtab(s->val.c);
			break;
			case START_COUNT:
				die("Config corruption in general.startup\n");
		}
	}

	// Mainloop
	g->run = 1;
	while (g->run) {
		if (g->newremotes) {
			pthread_mutex_lock(&g->remotemutex);
			g->newremotes = 0;
			u32 r = 0;
			const u32 max = g->remotes.size();
			for (; r < max; r++) {
				if (g->remotes[r].size) {
					printf(_("Opening remote URL %s\n"),
						g->remotes[r].data);
					newtab(g->remotes[r].data);
					free(g->remotes[r].data);
				} else {
					puts(_("Opening new tab, requested by remote"));
					newtab();
				}
			}
			g->remotes.clear();
			pthread_mutex_unlock(&g->remotemutex);
		}

		Fl::wait(0.5);
	}

	saveConfig();

	pthread_cancel(tid);
	pthread_join(tid, NULL);
	unlinkat(g->profilefd, LOCKFILE, 0);
	delete g->w;
	free(g->settings);
	delete g;
	return 0;
}
