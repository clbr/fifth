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
#include "lockicon.h"
#include <FL/Fl_PNG_Image.H>

static Fl_PNG_Image *lockicon;
static Fl_Input *search;

static void nextcb(Fl_Widget *, void *) {
	if (g->tabs[g->curtab].state != TS_WEB
		|| search->size() < 2)
		return;

	g->tabs[g->curtab].web->find(search->value());
}

static void prevcb(Fl_Widget *, void *) {
	if (g->tabs[g->curtab].state != TS_WEB
		|| search->size() < 2)
		return;

	g->tabs[g->curtab].web->find(search->value(), false, false);
}

statusbar::statusbar(int x, int y, int w, int h): Fl_Group(x, y, w, h) {
	lockicon = new Fl_PNG_Image("lock.png", lock_png, sizeof(lock_png));

	search = new Fl_Input(x + h, y + 1, 150, h - 2);
	next = new Fl_Button(x + h + 150 + 3, y + 1, 100, h - 2,
				_("Find next"));
	prev = new Fl_Button(x + h + 150 + 3 + 100 + 3, y + 1, 100, h - 2,
				_("Find previous"));

	hidefind();

	end();

	next->callback(nextcb);
	prev->callback(prevcb);
	search->callback(nextcb);
	search->when(FL_WHEN_CHANGED);

	::search = search;
}

void statusbar::draw() {
	u32 startx = x();
	const u32 endx = x() + w() - 1;

	if (damage() == FL_DAMAGE_CHILD) {
		draw_children();
		return;
	}

	// Background
	u32 i;
	const u32 max = h() - 2;
	u32 r1 = 209, g1 = 209, b1 = 209;
	u32 r2 = 120, g2 = 138, b2 = 147;
	for (i = 0; i <= max; i++) {
		const float pos = i / (float) max;

		fl_color(mix(r1, r2, pos),
			mix(g1, g2, pos),
			mix(b1, b2, pos));

		const u32 posy = y() + i;
		fl_line(startx, posy, endx, posy);
	}

	// Two border lines
	u32 posy = y() + max + 1;
	fl_color(79, 89, 100);
	fl_line(startx, posy, endx, posy);

	// SSL secure?
	const tab * const cur = &g->tabs[g->curtab];
	if (cur->url && !strncmp(cur->url, "https://", 8)) {
		const u32 secw = h();

		u32 ix, iy;
		ix = startx + (secw - lockicon->w()) / 2;
		iy = y() + (h() - lockicon->h()) / 2;

		lockicon->draw(ix, iy);

		startx += secw;
	}

	// If search not visible, draw status text, if any TODO
	if (!search->visible() && 0) {
	}

	draw_children();
}

int statusbar::handle(const int e) {
	return Fl_Group::handle(e);
}

void statusbar::startfind() {

	search->value("");

	search->show();
	next->show();
	prev->show();

	search->take_focus();
}

void statusbar::findnext() {
	nextcb(NULL, NULL);
}

void statusbar::findprev() {
	prevcb(NULL, NULL);
}

void statusbar::hidefind() {

	search->hide();
	next->hide();
	prev->hide();
}
