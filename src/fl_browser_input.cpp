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

#include <FL/Enumerations.H>
#include <FL/fl_draw.H>
#include <algorithm>

#include "fl_browser_input.h"

static void btn_cb(Fl_Widget *, void *ptr) {

	fl_browser_input *bi = (fl_browser_input *) ptr;

	if (bi->win->shown() && !bi->popping) {
		bi->win->hide();
		return;
	}

	// Calculate column widths
	static int widths[3] = {0, 0, 0};
	widths[0] = 0.65f * bi->w();
	bi->list->column_widths(widths);

	const u32 lines = bi->list->size();
	const u32 linepixels = lines * (bi->list->textsize() + 3) + 6;
	if (lines) {
		bi->list->value(1);
	} else {
		return;
	}

	// Calculate the window height and position
	const u32 bottom = bi->window()->y() + bi->y() + bi->h();
	const u32 halfh = Fl::h() / 2;
	const u32 fitting = Fl::h() - bottom;
	const u32 winh = std::min(std::min(halfh, fitting), linepixels);

	bi->win->size(bi->w(), winh);
	bi->win->position(bi->window()->x() + bi->x(), bottom);
	bi->win->show();
	bi->win->wait_for_expose();
}

static void brow_cb(Fl_Widget *, void *ptr) {

	fl_browser_input *bi = (fl_browser_input *) ptr;

	if (!bi->list->value())
		return;

	const char * const picked = bi->list->text(bi->list->value());
	const char * const end = strchr(picked, '\t');
	if (!end)
		return;
	const u32 len = end - picked;

	bi->inp->value(picked, len);
}

fl_browser_input::fl_browser_input(int x, int y, int w, int h): Fl_Group(x, y, w, h),
			popping(false) {

	box(FL_DOWN_BOX);

	x += Fl::box_dx(box());
	y += Fl::box_dy(box());
	w -= Fl::box_dw(box());
	h -= Fl::box_dh(box());

	inp = new inputplace(x, y, w - h, h);
	inp->box(FL_FLAT_BOX);
	inp->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED | FL_WHEN_CHANGED);

	but = new nicebtn(x + w - h, y, h, h);
	//but->label("@-32>");
	but->user_data(this);
	but->callback(btn_cb);
	but->clear_visible_focus();

	end();

	win = new Fl_Double_Window(10, 10);
	//win->parent(window());
	win->set_non_modal();
	win->clear_border();
	win->set_menu_window();

	list = new arrowbrow(0, 0, 10, 10);
	list->column_char('\t');
	list->user_data(this);
	list->callback(brow_cb);
	list->has_scrollbar(Fl_Browser_::VERTICAL_ALWAYS);
	win->resizable(list);

	win->end();
}

void fl_browser_input::resize(int x, int y, int w, int h) {

	Fl_Widget::resize(x, y, w, h);

	x += Fl::box_dx(box());
	y += Fl::box_dy(box());
	w -= Fl::box_dw(box());
	h -= Fl::box_dh(box());

	const u32 buttonw = 17;
	inp->resize(x, y, w - buttonw, h);
	but->resize(x + w - buttonw, y, buttonw, h);
}

int fl_browser_input::handle(const int e) {

	switch (e) {
		case FL_KEYDOWN:
			switch (Fl::event_key()) {
				case FL_Down:
					if (win->shown())
						win->take_focus();
					else
						but->do_callback();
					list->do_callback();
					return 1;
				break;
				case FL_Escape:
					win->hide();
				break;
			}
		break;
		case FL_UNFOCUS:
			if (win->shown() && !popping && Fl::focus() &&
				!win->contains(Fl::focus()) &&
				Fl::focus() != inp) {
				win->hide();
			}
		break;
	}

	return Fl_Group::handle(e);
}

int fl_browser_input::arrowbrow::handle(const int e) {

	fl_browser_input * const urlbar = (fl_browser_input *) user_data();
	inputplace * const inp = urlbar->inp;

	switch (e) {
		case FL_RELEASE:
			if (Fl::event_button() == FL_LEFT_MOUSE) {
				Fl_Hold_Browser::handle(e);
				const u32 sw = scrollbar_size() ?
						scrollbar_size() : Fl::scrollbar_size();
				const u32 ex = Fl::event_x();
				if (value() && ex < (x() + w() - sw)) {
					parent()->hide();
					inp->do_callback();
				}
			}
		break;
		case FL_KEYBOARD:
			switch (Fl::event_key()) {
				case FL_Enter:
					Fl_Hold_Browser::handle(e);
					if (value()) {
						do_callback();
						parent()->hide();
						inp->do_callback();
					}
					return 1;
				break;
				case FL_Up:
					if (value() == 1) {
						inp->take_focus();
						inp->position(inp->size());
						return 1;
					}
				break;
				case FL_Down:
					if (value() == size()) {
						value(1);
						return 1;
					}
				break;
				case FL_Escape:
					parent()->hide();
				break;
				case FL_Control_L:
				case FL_Control_R:
					urlbar->window()->handle(e);
					// Fallthrough
				default:
					parent()->hide();
					inp->handle(e);
				break;
			}
		break;
	}

	return Fl_Hold_Browser::handle(e);
}

void fl_browser_input::popup() {
	popping = true;

	but->do_callback();
	inp->take_focus();

	popping = false;
}

void fl_browser_input::hidewin() {
	if (win->shown())
		win->hide();
}
