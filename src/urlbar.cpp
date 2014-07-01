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

urlbar::urlbar(int x, int y, int w, int h): Fl_Widget(x, y, w, h) {

	prev = new urlbutton(0, 0, 0, 0);
	back = new urlbutton(0, 0, 0, 0);
	fwd = new urlbutton(0, 0, 0, 0);
	next = new urlbutton(0, 0, 0, 0);
	refresh = new urlbutton(0, 0, 0, 0);
	tabs = new urlbutton(0, 0, 0, 0);

	reposbuttons();
}

void urlbar::draw() {

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
	pos += diff;

	back->position(pos, posy);
	pos += diff;

	fwd->position(pos, posy);
	pos += diff;

	next->position(pos, posy);
	pos += diff;

	refresh->position(pos, posy);
	pos += diff;

	// Tabs on the other edge
	pos = x() + w() - 1 - dim - diff;
	prev->position(pos, posy);
}
