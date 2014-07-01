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
	if (image()) redraw();
	if (damage() & FL_DAMAGE_ALL) draw_box(b, color());

	if (image()) {
		const u32 iw = image()->w();
		Fl_Input_::drawtext(x() + Fl::box_dx(b) + iw + 6, y()+Fl::box_dy(b),
					w() - Fl::box_dw(b) - iw - 6, h()-Fl::box_dh(b));
		draw_label();
	} else {
		Fl_Input_::drawtext(x()+Fl::box_dx(b), y()+Fl::box_dy(b),
					w()-Fl::box_dw(b), h()-Fl::box_dh(b));
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
