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

#ifndef URL_H
#define URL_H

#include <FL/Fl_Group.H>

#include "urlbutton.h"
#include "textweb.h"

class Fl_PNG_Image;

class urlbar: public Fl_Group {
public:
	urlbar(int x, int y, int w, int h);

	void draw() override;
	void resize(int x, int y, int w, int h) override;

	void refreshstate(const bool refresh);
	bool isStop() const;

	urlbutton *prev, *back, *fwd, *next, *refresh, *tabs;
	textweb *url, *search;
private:
	void reposbuttons();

	Fl_PNG_Image *refreshimg, *stopimg;
};

void urlbuttonstate();

#endif
