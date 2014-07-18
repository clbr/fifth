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

statusbar::statusbar(int x, int y, int w, int h): Fl_Group(x, y, w, h) {
	lockicon = new Fl_PNG_Image("lock.png", lock_png, sizeof(lock_png));
}

void statusbar::draw() {
	u32 startx = x();
	const u32 endx = x() + w() - 1;

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

	// SSL secure? TODO
	if (1) {
		const u32 secw = h();

		u32 ix, iy;
		ix = startx + (secw - lockicon->w()) / 2;
		iy = y() + (h() - lockicon->h()) / 2;

		lockicon->draw(ix, iy);

		startx += secw;
	}
}

int statusbar::handle(const int e) {
	return Fl_Group::handle(e);
}
