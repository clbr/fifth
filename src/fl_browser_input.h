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

#ifndef FL_BROWSER_INPUT
#define FL_BROWSER_INPUT

#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include "inputplace.h"
#include <lrtypes.h>

class fl_browser_input: public Fl_Group {
public:
	fl_browser_input(int x, int y, int w, int h);
	virtual ~fl_browser_input() {}

	int handle(int) override;

	void resize(int, int, int, int) override;

	class arrowbrow: public Fl_Hold_Browser {
	public:
		arrowbrow(int x, int y, int w, int h): Fl_Hold_Browser(x, y, w, h) {}

		int handle(int) override;
	};

	void popup();
	void hidewin();

	inputplace &input() const { return *inp; }

	Fl_Double_Window *win;
	arrowbrow *list;
	inputplace *inp;

	bool popping;

private:
	class nicebtn: public Fl_Button {
	public:
		nicebtn(int x, int y, int w, int h): Fl_Button(x, y, w, h) {}

		void draw() override {
			draw_box(FL_UP_BOX, color());
			fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
			int xc = x()+w()/2, yc=y()+h()/2;
			fl_polygon(xc-4,yc-2,xc+4,yc-2,xc,yc+2);
			if (Fl::focus() == this) draw_focus();
		}
	};

	nicebtn *but;
};

#endif
