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

#ifndef VIEW_H
#define VIEW_H

#include <FL/Fl_Group.H>

class Fl_Button;
class Fl_Input;
class Fl_Hold_Browser;
class sslview;

struct dl {
	const char *name;
	const char *url;
	long long size;
	long long received;
	time_t start;

	webview *owner;
	u32 id;

	bool finished;
	bool failed;
};

class view: public Fl_Group {
public:
	view(int x, int y, int w, int h);

	void draw() override;
	void resize(int, int, int, int) override;
	int handle(int) override;

	void resetssl();
	void refreshdownloads();

	const dl *selecteddl() const;
private:
	void drawdial();
	void drawssl();
	void drawdl();

	void dialdims(u32 *, u32 *, u32 *, u32 *, u32 *, u32 *, u32 *) const;
	void regendl(const std::vector<dl> &vec);

	u32 mousex, mousey;
	bool mousein;

	u32 downloads;

	sslview *sslgroup;
	Fl_Button *sslbutton;
	Fl_Input *ssltext;

	Fl_Group *dlgroup;
	Fl_Button *dlstop, *dlredo;
	Fl_Hold_Browser *dlbrowser;
	std::vector<dl> lastdls;
};

std::vector<dl> getdownloads();

#endif
