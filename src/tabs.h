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
};

enum tabstate {
	TS_WEB,
	TS_DOWNLOAD,
	TS_SSLERR,
	TS_SPEEDDIAL,
	TS_COUNT
};

struct tab {
	tabstate state;
	webview *web;
	u64 lastactive;
	Fl_RGB_Image *icon;

	tab();
	const char *title();
};

void newtab();
void newtab(const char *url);
void newtabbg(const char *url);
void closetab();

void prevtab();
void nexttab();

void activatetab(const u16);

std::vector<u16> taborder();

#endif
