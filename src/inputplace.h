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

#ifndef INPUTPLACE_H
#define INPUTPLACE_H

#include <FL/Fl_Input.H>

class inputplace: public Fl_Input {
public:
	inputplace(int x, int y, int w, int h);
	~inputplace();

	int handle(int) override;
	void draw() override;

	void placeholder(const char *);
private:
	const char *placeholdertext;
};

#endif
