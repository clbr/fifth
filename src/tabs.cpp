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

tabbar::tabbar(int x, int y, int w, int h): Fl_Widget(x, y, w, h) {

}

void tabbar::draw() {
	const u32 startx = x();
	const u32 endx = x() + w() - 1;

	const u32 gradarea = h() - 2;
	const u32 gradhalf = gradarea / 2;
	u32 i;

	// Upper half bg
	u32 r1 = 63, g1 = 72, b1 = 81;
	u32 r2 = 50, g2 = 60, b2 = 70;
	for (i = 0; i <= gradarea; i++) {
		const float pos = i / (float) gradhalf;

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
	const u32 defw = 150, minw = 50;
	u32 max = g->tabs.size();
	u32 tabw = defw;

	if (tabw * max >= (u32) w()) {
		tabw = w() / max;
		while (tabw < minw) {
			max--;
			tabw = w() / max;
		}
	}

	u32 posx = x();
	for (i = 0; i < max; i++) {
		printf("tab %u, %s\n", i, g->tabs[i].title());
	}
}

tab::tab(): state(TS_WEB), web(NULL), lastactive(msec()) {

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
		u16 next = order[0];
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

vector<u16> taborder() {
	// Return the indices of all active tabs sans the current one,
	// ordered by their access times in descending order. TODO

	vector<u16> out;
	const u16 max = g->tabs.size();
	out.reserve(max);

	u16 i;
	for (i = 0; i < max; i++) {
		if (i == g->curtab)
			continue;

		out.push_back(i);
	}

	return out;
}
