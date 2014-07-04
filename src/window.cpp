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

void window::hide() {
	g->run = 0;

	// Save the last used window position and size
	setting *s = getSetting("window.x", NULL);
	s->val.u = x();
	s = getSetting("window.y", NULL);
	s->val.u = y();
	s = getSetting("window.w", NULL);
	s->val.u = w();
	s = getSetting("window.h", NULL);
	s->val.u = h();

	// Close all tabs
	u32 max = g->tabs.size();
	u32 i;
	for (i = 0; i < max; i++) {
		if (g->tabs[i].web) {
			g->tabs[i].web->parent()->remove(g->tabs[i].web);
			delete g->tabs[i].web;
		}
	}

	max = g->closedtabs.size();
	for (i = 0; i < max; i++) {
		if (g->closedtabs[i].web) {
			g->closedtabs[i].web->parent()->remove(g->closedtabs[i].web);
			delete g->closedtabs[i].web;
		}
	}

	Fl_Widget::hide();
}

static void cb(Fl_Widget *w, void*) {
	if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
		return;
	w->hide();
}

window::window(int x, int y, int w, int h): Fl_Double_Window(x, y, w, h) {
	callback(cb);
	color(fl_rgb_color(63, 72, 81));
}

int window::handle(const int e) {
	if (e == FL_KEYDOWN) {
		unsigned key = Fl::event_key();
		if (key == FL_Control_L || key == FL_Control_R) startctrl();
	} else if (e == FL_KEYUP) {
		unsigned key = Fl::event_key();
		if (key == FL_Control_L || key == FL_Control_R) endctrl();
	}

	if (e == FL_SHORTCUT) {
		keybinding key;

		key.key = Fl::event_key();
		key.ctrl = Fl::event_ctrl();
		key.alt = Fl::event_alt();
		key.shift = Fl::event_shift();

		if (g->keys.count(key) == 0) {
			if (g->debugkeys)
				printf("Unrecognized shortcut %u%s%s%s (%u, %c)\n", key.key,
					key.ctrl ? " + ctrl" : "",
					key.alt ? " + alt" : "",
					key.shift ? " + shift" : "",
					keytou32(key),
					key.key);

			return Fl_Window::handle(e);
		}

		g->keys[key]();

		return 1;
	}

	return Fl_Window::handle(e);
}
