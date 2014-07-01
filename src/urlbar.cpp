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
#include "urlicons.h"
#include "textweb.h"
#include <FL/Fl_PNG_Image.H>

urlbar::urlbar(int x, int y, int w, int h): Fl_Group(x, y, w, h) {

	prev = new urlbutton(0, 0, 0, 0);
	back = new urlbutton(0, 0, 0, 0);
	fwd = new urlbutton(0, 0, 0, 0);
	next = new urlbutton(0, 0, 0, 0);
	refresh = new urlbutton(0, 0, 0, 0);

	url = new textweb(0, 0, 0, 0);
	search = new textweb(0, 0, 0, 0);

	tabs = new urlbutton(0, 0, 0, 0);

	end();

	reposbuttons();

	// TODO: theming
	#define img(a) a, sizeof(a)
	refreshimg = new Fl_PNG_Image("refresh.png", img(reload_png));
	stopimg = new Fl_PNG_Image("stop.png", img(stop_png));

	prev->image(new Fl_PNG_Image("prev.png", img(twoleftarrow_png)));
	back->image(new Fl_PNG_Image("back.png", img(leftarrow_png)));
	fwd->image(new Fl_PNG_Image("fwd.png", img(rightarrow_png)));
	next->image(new Fl_PNG_Image("next.png", img(tworightarrow_png)));

	prev->deimage(new Fl_PNG_Image("deprev.png", img(detwoleftarrow_png)));
	back->deimage(new Fl_PNG_Image("deback.png", img(deleftarrow_png)));
	fwd->deimage(new Fl_PNG_Image("defwd.png", img(derightarrow_png)));
	next->deimage(new Fl_PNG_Image("denext.png", img(detworightarrow_png)));

	refresh->image(refreshimg);
	tabs->image(new Fl_PNG_Image("tabs.png", img(tabs_png)));
	#undef img

	url->input().placeholder("WWW address...");
	search->input().placeholder("DuckDuckGo");
}

void urlbar::draw() {

	if (damage() == FL_DAMAGE_CHILD) {
		draw_children();
		return;
	}

	const u32 startx = x();
	const u32 endx = x() + w() - 1;

	// Background
	u32 i;
	const u32 max = h() - 3;
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

	posy = y() + max + 2;
	fl_color(25, 35, 45);
	fl_line(startx, posy, endx, posy);

	// Kids
	draw_children();
}

void urlbar::resize(int x, int y, int w, int h) {

	Fl_Widget::resize(x, y, w, h);

	// Reposition buttons
	reposbuttons();
}

void urlbar::reposbuttons() {
	const u32 diff = 4;
	const u32 ydiff = 3;
	const u32 dim = h() - (ydiff * 2);

	prev->size(dim, dim);
	back->size(dim, dim);
	fwd->size(dim, dim);
	next->size(dim, dim);
	refresh->size(dim, dim);
	tabs->size(dim, dim);

	// TODO: removable buttons
	u32 pos = x() + diff;
	const u32 posy = y() + ydiff;

	prev->position(pos, posy);
	pos += diff + dim;

	back->position(pos, posy);
	pos += diff + dim;

	fwd->position(pos, posy);
	pos += diff + dim;

	next->position(pos, posy);
	pos += diff + dim;

	refresh->position(pos, posy);
	pos += diff + dim;

	const u32 urlstart = pos;

	// Tabs on the other edge
	pos = x() + w() - 1 - dim - diff;
	tabs->position(pos, posy);

	const u32 searchw = 170;
	pos -= diff + searchw;
	search->resize(pos, posy, searchw, dim);

	pos -= diff;

	url->resize(urlstart, posy, pos - urlstart, dim);
}
