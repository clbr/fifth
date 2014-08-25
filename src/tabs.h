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

#ifndef TABS_H
#define TABS_H

#include <FL/Fl_Widget.H>
#include <vector>

class webview;

class tabbar: public Fl_Widget {
public:
	tabbar(int x, int y, int w, int h);

	void draw() override;
	int handle(int e) override;

private:
	u32 mousex;
	bool mousein;
	bool dragging;
	u32 srctab;
};

enum tabstate {
	TS_WEB,
	TS_DOWNLOAD,
	TS_SSLERR,
	TS_SPEEDDIAL,
	TS_COUNT
};

enum tabsearch {
	TSE_DDG,
	TSE_GOOGLE,
	TSE_COUNT
};

enum tabtristate {
	TRI_OFF = 0,
	TRI_ON,
	TRI_AUTO
};

struct tab {
	tabstate state;
	tabsearch engine;

	webview *web;
	u64 lastactive;
	Fl_RGB_Image *icon;

	const char *url;
	const char *search;
	const char *sslsite;

	float progress;

	tabtristate css;
	tabtristate js;
	tabtristate img;

	tab();
	const char *title() const;
};

void newtab();
void newtab(const char *url);
void newtabbg(const char *url);
void closetab();
void undotab();

void prevtab();
void nexttab();

void activatetab(const u16);

void startctrl();
void endctrl();

std::vector<u16> taborder();

tab *findtab(const webview *, bool closedok = false);

#endif
