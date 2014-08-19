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

#ifndef MAIN_H
#define MAIN_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <vector>
#include <string>
#include <map>
#include <fcntl.h>
#include <physfs.h>
#include <pthread.h>
#include <locale.h>
#include <limits.h>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Menu_Bar.H>
#include <webkit.h>
#include <iconv.h>

#include "autoconfig.h"
#include "helpers.h"
#include "settings.h"
#include "macros.h"
#include "config.h"
#include "gettext.h"
#include "remote.h"
#include "window.h"
#include "status.h"
#include "tabs.h"
#include "urlbar.h"
#include "view.h"
#include "inputplace.h"
#include "keys.h"
#include "bookmarks.h"
#include "ssl.h"
#include "blocking.h"
#include "about.h"
#include "history.h"

using namespace std;

struct globals {
	const char *profilepath;
	int profilefd;
	int datafd;
	int lockfd;
	int certfd;

	setting *settings;
	map<string, vector<setting> > sitesettings;

	vector<remotemsg> remotes;
	u8 newremotes;
	pthread_mutex_t remotemutex;

	bool bench;
	bool run;
	bool debugkeys;

	window *w;
	view *v;
	urlbar *url;
	tabbar *tabwidget;
	statusbar *status;
	Fl_Menu_Bar *menu;

	vector<tab> tabs;
	vector<tab> closedtabs;
	u32 curtab;
	vector<webview *> dlwebs;

	map<keybinding, keyfunc> keys;

	vector<bookmark> bookmarks;

	urlctx *blacklist;
	urlctx *whitelist;

	histbuf *history;

	iconv_t conv;
};

extern struct globals *g;

enum startup {
	START_DIAL = 0,
	START_HOME,
	START_COUNT
};

void generatemenu();

enum crashchoice {
	CRASH_RESTORE = 0,
	CRASH_NORMAL,
	CRASH_BLANK,
	CRASH_COUNT
};

crashchoice crashdialog();

#endif
