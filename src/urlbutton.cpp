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
#include "urlbutton.h"

urlbutton::urlbutton(int x, int y, int w, int h) : Fl_Button(x, y, w, h) {

}

void urlbutton::draw() {
	Fl_Color col = value() ? selection_color() : color();
	draw_box(value() ? FL_DOWN_FRAME : FL_UP_FRAME, col);
	draw_label();
	if (Fl::focus() == this) draw_focus();
}

int urlbutton::handle(const int e) {

	// We have a transparent button. If we lose focus, request bg redraw.
	if (e == FL_FOCUS || e == FL_UNFOCUS) {
		int X = x() > 0 ? x() - 1 : 0;
		int Y = y() > 0 ? y() - 1 : 0;
		if (window()) window()->damage(FL_DAMAGE_ALL, X, Y, w() + 2, h() + 2);
		return 1;
	}

	tab * const cur = &g->tabs[g->curtab];
	if (e == FL_KEYDOWN && cur->state == TS_WEB &&
		(Fl::event_key() == FL_Left ||
		Fl::event_key() == FL_Right ||
		Fl::event_key() == FL_Up ||
		Fl::event_key() == FL_Down ||
		Fl::event_key() == FL_Page_Down ||
		Fl::event_key() == FL_Page_Up)) {

		// For example, pressing page down after clearing the trash.
		cur->web->take_focus();
		return cur->web->handle(e);
	}

	return Fl_Button::handle(e);
}
