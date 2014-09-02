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

	int handle(int);

	class arrowbrow: public Fl_Hold_Browser {
	public:
		arrowbrow(int x, int y, int w, int h): Fl_Hold_Browser(x, y, w, h) {}

		int handle(int);
	};

	Fl_Double_Window *win;
	arrowbrow *list;
	inputplace *inp;

private:
	Fl_Button *but;
};

#endif