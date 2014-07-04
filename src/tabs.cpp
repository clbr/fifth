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

static vector<u16> longorder;
static u16 longpress;
static bool firstpress;

tabbar::tabbar(int x, int y, int w, int h): Fl_Widget(x, y, w, h),
		mousex(0), mousein(false) {
	labelsize(12);
}

static u32 calctabw(u32 *outmax, const u32 w) {

	const u32 defw = 150, minw = 50;
	u32 max = g->tabs.size();
	u32 tabw = defw;

	if (tabw * max >= w) {
		tabw = w / max;
		while (tabw < minw) {
			max--;
			tabw = w / max;
		}
	}

	*outmax = max;
	return tabw;
}

void tabbar::draw() {
	const u32 startx = x();
	const u32 endx = x() + w() - 1;

	const u32 gradarea = h() - 2;
	u32 i, j;

	// bg
	u32 r1 = 63, g1 = 72, b1 = 81;
	u32 r2 = 38, g2 = 48, b2 = 59;
	for (i = 0; i <= gradarea; i++) {
		const float pos = i / (float) gradarea;

		fl_color(mix(r1, r2, pos),
			mix(g1, g2, pos),
			mix(b1, b2, pos));

		const u32 posy = y() + 1 + i;
		fl_line(startx, posy, endx, posy);
	}

	// Borders
	fl_color(37, 46, 52);
	fl_line(startx, y(), endx, y());
	fl_line(startx, y() + h() - 1, endx, y() + h() - 1);

	// Tabs
	u32 max;
	u32 tabw = calctabw(&max, w());

	u32 posx = x();
	const u32 lowy = y();
	const u32 highy = y() + h() - 1;
	for (i = 0; i < max; i++) {
		bool hover = false;
		// Borders
		fl_color(FL_BLACK);
		fl_line(posx, lowy, posx, highy);
		fl_line(posx + tabw, lowy, posx + tabw, highy);

		// Bg
		r1 = 83, g1 = 92, b1 = 101;
		r2 = 58, g2 = 68, b2 = 79;

		if (i == g->curtab) {
			r1 = 100, g1 = 160, b1 = 191;
			r2 = 42, g2 = 100, b2 = 125;
		} else if (mousein && mousex >= posx && mousex <= (posx + tabw - 1)) {
			r1 = 150, g1 = 150, b1 = 191;
			r2 = 100, g2 = 100, b2 = 155;
			hover = true;
		}

		for (j = 0; j <= gradarea; j++) {
			const float pos = j / (float) gradarea;

			fl_color(mix(r1, r2, pos),
				mix(g1, g2, pos),
				mix(b1, b2, pos));

			const u32 posy = y() + 1 + j;
			fl_line(posx + 1, posy, posx + tabw - 1, posy);
		}

		// Icon
		Fl_Image *img = g->tabs[i].icon;
		if (!img)
			img = Fl_Shared_Image::get("newtab.png");

		const u32 imgy = y() + (h() - 16) / 2;
		img->draw(posx + 4, imgy, 16, 16);

		// Text
		fl_color(200, 200, 200);
		if (i == g->curtab)
			fl_color(FL_WHITE);
		else if (hover)
			fl_color(230, 230, 230);
		fl_font(labelfont(), labelsize());

		char tmp[80];
		memset(tmp, 0, 80);
		strncpy(tmp, g->tabs[i].title(), 79);

		int textw = 0, texth = 0;
		fl_measure(tmp, textw, texth, 0);

		const u32 textarea = tabw - 2 - 8 - 16;
		while ((u32) textw >= textarea) {
			// Shorten it until it fits
			const u32 len = strlen(tmp) - 1;
			tmp[len] = '\0';
			tmp[len - 1] = '.';
			tmp[len - 2] = '.';
			tmp[len - 3] = '.';

			textw = texth = 0;
			fl_measure(tmp, textw, texth, 0);
		}

		const u32 texty = y() - fl_descent() + fl_height() + (h() - fl_height())/2;
		fl_draw(tmp, posx + 2 + 8 + 16, texty);

		posx += tabw;
	}
}

int tabbar::handle(const int e) {

	bool ontab = false;
	u32 which = 0;

	switch (e) {
		case FL_ENTER:
			mousein = true;
			return 1;
		case FL_LEAVE:
			mousein = false;
			redraw();
			return 1;
		case FL_MOVE:
			mousex = Fl::event_x();
			redraw();
			return 1;
		case FL_PUSH:
			// Over a tab?
			if (mousein) {
				u32 max;
				const u32 tabw = calctabw(&max, w());

				if (mousex < max * tabw) {
					ontab = true;
					u32 tmp = mousex - x();
					tmp /= tabw;
					which = tmp;
				}
			}

			// Double click on empty?
			if (Fl::event_clicks() && Fl::event_button() == FL_LEFT_MOUSE &&
				!ontab) {
				newtab();
				return 1;
			}

			if (ontab) {
				if (Fl::event_button() == FL_LEFT_MOUSE) {
					activatetab(which);
				} else if (Fl::event_button() == FL_MIDDLE_MOUSE) {
					g->curtab = which;
					closetab();
				}
			}

			return 1;
	}

	return Fl_Widget::handle(e);
}

tab::tab(): state(TS_WEB), web(NULL), lastactive(msec()), icon(NULL) {

}

const char *tab::title() {
	switch (state) {
		case TS_WEB:
			return web->title();
		break;
		case TS_DOWNLOAD:
			return _("Downloads");
		break;
		case TS_SPEEDDIAL:
			return _("Speed dial");
		break;
		case TS_SSLERR:
			return _("Security risk");
		break;
		case TS_COUNT:
			die("Tab corruption\n");
	}

	die("Tab corruption\n");
}

void newtab() {
	Fl_Group::current(0);
	tab tab;
	tab.state = TS_SPEEDDIAL;
	tab.web = new webview(g->v->x(), g->v->y(), g->v->w(), g->v->h());
	g->tabs.push_back(tab);
	g->curtab = g->tabs.size() - 1;
	g->w->redraw();
}

void closetab() {
	if (g->tabs.size() == 1) {

		if (g->tabs[0].state == TS_WEB) {
			g->closedtabs.push_back(g->tabs[0]);
		} else {
			if (g->tabs[0].web)
				delete g->tabs[0].web;
		}

		g->tabs.clear();
		newtab();
	} else {
		const vector<u16> &order = taborder();
		u16 next = order[1];
		if (next > g->curtab)
			next--;

		if (g->tabs[0].state == TS_WEB) {
			g->closedtabs.push_back(g->tabs[g->curtab]);
		} else {
			if (g->tabs[g->curtab].web)
				delete g->tabs[g->curtab].web;
		}

		g->tabs.erase(g->tabs.begin() + g->curtab);
		g->curtab = next;
		g->w->redraw();
	}
}

void newtab(const char *url) {
	Fl_Group::current(0);
	tab tab;
	tab.web = new webview(g->v->x(), g->v->y(), g->v->w(), g->v->h());
	g->tabs.push_back(tab);
	g->curtab = g->tabs.size() - 1;
	g->w->redraw();

	tab.web->load(url);
}

void newtabbg(const char *url) {
	Fl_Group::current(0);
	tab tab;
	tab.web = new webview(g->v->x(), g->v->y(), g->v->w(), g->v->h());
	tab.lastactive = 0; // Put it in the back of the queue

	g->tabs.push_back(tab);

	tab.web->load(url);
}

struct tmptab {
	u64 time;
	u16 i;
};

static int tabcmp(const void *ap, const void *bp) {
	const tmptab * const a = (const tmptab *) ap;
	const tmptab * const b = (const tmptab *) bp;

	if (a->time > b->time)
		return -1;
	else if (a->time < b->time)
		return 1;
	return 0;
}

vector<u16> taborder() {
	// Return the indices of all active tabs,
	// ordered by their access times in descending order.

	vector<u16> out;
	const u16 max = g->tabs.size();
	out.reserve(max);

	tmptab arr[max];

	u16 i, k = 0;
	for (i = 0; i < max; i++) {
		arr[k].time = g->tabs[i].lastactive;
		arr[k].i = i;
		k++;
	}

	qsort(arr, max, sizeof(struct tmptab), tabcmp);

	for (i = 0; i < max; i++) {
		out.push_back(arr[i].i);
	}

	return out;
}

void nexttab() {
	if (g->tabs.size() == 1)
		return;

	longpress++;
	longpress %= longorder.size();
	firstpress = false;
}

void prevtab() {
	if (g->tabs.size() == 1)
		return;

	if (longpress)
		longpress--;
	else
		longpress = longorder.size() - 1;
	firstpress = false;
}

void activatetab(const u16 tab) {

	if (g->tabs[g->curtab].web)
		g->tabs[g->curtab].web->hide();

	g->curtab = tab;
	g->tabs[g->curtab].lastactive = msec();

	if (g->tabs[g->curtab].web)
		g->tabs[g->curtab].web->show();

	g->w->redraw();
}

void startctrl() {
	longorder = taborder();
	longpress = 0;
	firstpress = true;
}

void endctrl() {
	if (!firstpress)
		activatetab(longorder[longpress]);
	longorder.clear();
}
