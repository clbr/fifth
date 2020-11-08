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
		drawprogress(false), placeholdertext(NULL), adjustingdrag(false) {
	align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
}

inputplace::~inputplace() {
	free((char *) placeholdertext);
}

void inputplace::draw() {
	const Fl_Boxtype b = box();
	if (image() || placeholdertext) redraw();
	if (damage() & FL_DAMAGE_ALL) draw_box(b, color());

	u32 xoff = 0;

	if (image() && !adjustingdrag) {
		const u32 iw = image()->w();
		xoff = iw + 6;
		draw_label();
	}

	fl_font(textfont(), textsize());

	Fl_Input_::drawtext(x() + Fl::box_dx(b) + xoff, y()+Fl::box_dy(b),
				w() - Fl::box_dw(b) - xoff, h()-Fl::box_dh(b));

	if (placeholdertext && (!value() || strlen(value()) < 1) &&
		Fl::focus() != this) {
		fl_color(150, 150, 150);
		fl_draw(placeholdertext, x() + Fl::box_dx(b) + xoff + 1,
			y() - fl_descent() + fl_height() + (h() - fl_height())/2 + 1);
	}

	const float prog = g->tabs[g->curtab].progress;

	if (drawprogress && g->tabs[g->curtab].state == TS_WEB && prog < 99.9f) {
		const u32 dx = x() + w() - 101;
		const u32 dy = y() + Fl::box_dy(b);
		const u32 dw = 100 - Fl::box_dw(b);
		const u32 dh = h() - Fl::box_dh(b);

		fl_color(20, 30, 60);
		fl_rectf(dx, dy, dw, dh);

		const u32 neww = dw * (prog * 0.01f);
		fl_color(70, 80, 110);
		fl_rectf(dx + 1, dy + 1, neww, dh - 1);

		fl_color(FL_WHITE);
		char tmp[10];
		snprintf(tmp, 10, "%.1f%%", prog);
		int textw = 0, texth = 0;
		fl_measure(tmp, textw, texth);

		fl_draw(tmp, dx + (dw - textw) / 2,
			y() - fl_descent() + fl_height() + (h() - fl_height())/2 + 1);
	}
}

int inputplace::handle(const int e) {

	if (e == FL_KEYBOARD) {
		switch (Fl::event_key()) {
			case FL_Down:
			case FL_Up:
				// Send it up so our parent may use it
				return 0;
			break;
			case FL_Enter:
				ctrl = !!Fl::event_ctrl();
				shift = !!Fl::event_shift();
				// Fallthrough
			break;
		}
	} else if ((e == FL_PUSH || e == FL_DRAG || e == FL_DND_DRAG) && image()) {
		if (!adjustingdrag) { // may recurse within the same frame
			adjustingdrag = true;
			const int oldx = x();
			const int oldw = w();
			resize(oldx + image()->w() + 6, y(), w() - image()->w() - 6, h());
			const int ret = Fl_Input::handle(e);
			resize(oldx, y(), oldw, h());
			adjustingdrag = false;
			return ret;
		} else {
			return Fl_Input::handle(e);
		}
	}

	return Fl_Input::handle(e);
}

void inputplace::placeholder(const char *s) {
	free((char *) placeholdertext);
	placeholdertext = strdup(s);
}
