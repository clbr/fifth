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
	for (i = 0; i <= gradhalf; i++) {
		const float pos = i / (float) gradhalf;

		fl_color(mix(r1, r2, pos),
			mix(g1, g2, pos),
			mix(b1, b2, pos));

		if (i == gradhalf) {
			fl_color(40, 50, 60);
		}

		const u32 posy = y() + 1 + i;
		fl_line(startx, posy, endx, posy);
	}

	// Lower half bg
	r1 = 30, g1 = 40, b1 = 50;
	r2 = 45, g2 = 55, b2 = 65;
	for (i = 0; i <= gradhalf; i++) {
		const float pos = i / (float) gradhalf;

		fl_color(mix(r1, r2, pos),
			mix(g1, g2, pos),
			mix(b1, b2, pos));

		const u32 posy = y() + 2 + i + gradhalf;
		fl_line(startx, posy, endx, posy);
	}

	// Borders
	fl_color(37, 46, 52);
	fl_line(startx, y(), endx, y());
	fl_line(startx, y() + h() - 1, endx, y() + h() - 1);
}
