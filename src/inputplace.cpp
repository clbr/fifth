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

inputplace::inputplace(int x, int y, int w, int h): Fl_Input(x, y, w, h),
		placeholdertext(NULL) {
	align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
}

void inputplace::draw() {
	const Fl_Boxtype b = box();
	if (image() || placeholdertext) redraw();
	if (damage() & FL_DAMAGE_ALL) draw_box(b, color());

	u32 xoff = 0;

	if (image()) {
		const u32 iw = image()->w();
		xoff = iw + 6;
		draw_label();
	}

	Fl_Input_::drawtext(x() + Fl::box_dx(b) + xoff, y()+Fl::box_dy(b),
				w() - Fl::box_dw(b) - xoff, h()-Fl::box_dh(b));

	if (placeholdertext && (!value() || strlen(value()) < 1) &&
		Fl::focus() != this) {
		fl_color(150, 150, 150);
		fl_draw(placeholdertext, x() + Fl::box_dx(b) + xoff + 1,
			y() - fl_descent() + fl_height() + (h() - fl_height())/2);
	}
}

int inputplace::handle(const int e) {
	if (e == FL_KEYBOARD) {
		if (Fl::event_key() == FL_Down) {
			// Send it up so our parent may use it
			return 0;
		}
	}

	return Fl_Input::handle(e);
}

void inputplace::placeholder(const char *s) {
	free((char *) placeholdertext);
	placeholdertext = strdup(s);
}
