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
#include "zoomer.h"

zoomer::zoomer(int x, int y, int w, int h): Fl_Roller(x, y, w, h) {
	tooltip(_("Page zoom. Double-click to return to 100%."));
	range(0.1, 10);
	value(1);
	step(1, 20);
}

void zoomer::draw() {
	Fl_Roller::draw();

	const float val = value() * 100;

	fl_font(FL_HELVETICA, 12);

	char tmp[10];
	snprintf(tmp, 10, "%.0f%%", val);

	int tw = 0, th = 0, tx, ty;
	fl_measure(tmp, tw, th, 0);

	tx = x() + (w() - tw) / 2;
	ty = y() + (h() - th) / 2;

	fl_color(FL_GRAY);
	fl_rectf(tx, ty, tw, th);

	fl_color(FL_BLACK);
	fl_draw(tmp, tx, ty - fl_descent() + fl_height());
}

int zoomer::handle(const int e) {

	if (e == FL_PUSH && Fl::event_clicks()) {
		value(1);
		do_callback();
		return 1;
	}

	return Fl_Roller::handle(e);
}
