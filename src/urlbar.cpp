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

}

void urlbar::draw() {

	const u32 startx = x();
	const u32 endx = x() + w() - 1;

	// Background
	u32 i;
	const u32 max = h() - 3;
	u32 r1 = 240, g1 = 240, b1 = 240;
	u32 r2 = 190, g2 = 200, b2 = 205;
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
	fl_color(167, 173, 179);
	fl_line(startx, posy, endx, posy);

	posy = y() + max + 2;
	fl_color(125, 135, 145);
	fl_line(startx, posy, endx, posy);
}
