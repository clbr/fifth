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

}

void inputplace::draw() {
	Fl_Input::draw();
}

int inputplace::handle(const int e) {
	if (e == FL_KEYBOARD) {
		if (Fl::event_key() == FL_Down) {
			return 1;
		}
	}

	return Fl_Input::handle(e);
}

void inputplace::placeholder(const char *s) {
	free((char *) placeholdertext);
	placeholdertext = strdup(s);
}
