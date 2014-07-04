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

view::view(int x, int y, int w, int h): Fl_Group(x, y, w, h) {
	end();
}

void view::draw() {

	tab * const cur = &g->tabs[g->curtab];

	// TODO drawing
	switch (cur->state) {
		case TS_WEB:
			if (cur->web)
				cur->web->draw();
		break;
		case TS_DOWNLOAD:
		break;
		case TS_SSLERR:
		break;
		case TS_SPEEDDIAL:
		break;
		case TS_COUNT:
			die("Corrupted tab state\n");
	}
}

void view::resize(int x, int y, int w, int h) {
	Fl_Widget::resize(x, y, w, h);

	// Resize all webviews
	u32 max = g->tabs.size();
	u32 i;
	for (i = 0; i < max; i++) {
		if (g->tabs[i].web)
			g->tabs[i].web->resize(x, y, w, h);
	}

	max = g->closedtabs.size();
	for (i = 0; i < max; i++) {
		if (g->closedtabs[i].web)
			g->closedtabs[i].web->resize(x, y, w, h);
	}
}

int view::handle(const int e) {

	if (g->tabs.size() < 1)
		return Fl_Widget::handle(e);

	tab * const cur = &g->tabs[g->curtab];

	if (cur->state == TS_WEB && cur->web)
		return cur->web->handle(e);

	switch (cur->state) {
		case TS_DOWNLOAD:
		break;
		case TS_SSLERR:
		break;
		case TS_SPEEDDIAL:
		break;
		case TS_WEB:
		break;
		case TS_COUNT:
			die("Not reached\n");
	}

	return Fl_Widget::handle(e);
}
