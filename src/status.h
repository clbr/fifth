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

#ifndef STATUS_H
#define STATUS_H

#include <FL/Fl_Group.H>

class Fl_Input;
class Fl_Button;
class Fl_Box;
class Fl_Roller;

class statusbar: public Fl_Group {
public:
	statusbar(int x, int y, int w, int h);

	void draw() override;
	int handle(int e) override;

	void startfind();
	void findnext();
	void findprev();
	void hidefind();

	void resize(int, int, int, int) override;

	void refreshzoom();
	void wheelzoom();

	bool externalzoom;
private:
	void reposbuttons();

	Fl_Input *search;
	Fl_Button *next, *prev;
	Fl_Box *total;

	Fl_Button *js, *css, *img;
	Fl_Roller *zoom;
};

#endif
