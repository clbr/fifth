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
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Button.H>

static Fl_Input *dialurl=(Fl_Input *)0;
static Fl_Double_Window* w;
static Fl_Choice* choice;
static setting *target;

static void act() {
	target->val.c = strdup(dialurl->value());
	g->v->redraw();
}

static void cb_OK(Fl_Return_Button*, void*) {
	w->hide();
	act();
}

static void cb_Cancel(Fl_Button*, void*) {
	w->hide();
}

static void cb_Choice(Fl_Widget*, void*) {
	const Fl_Menu_Item * const val = choice->mvalue();
	dialurl->value(g->tabs[(uintptr_t) val->user_data_].url);
}

void adddial(const u32 which) {
	if (!w) {
		Fl_Double_Window* o = new Fl_Double_Window(420, 125, _("Add speed dial entry"));
		w = o;
		{ dialurl = new Fl_Input(90, 15, 305, 25, _("URL:"));
		} // Fl_Input* dialurl
		{ Fl_Choice* o = new Fl_Choice(90, 45, 305, 25, _("Open tabs:"));
			o->down_box(FL_BORDER_BOX);
			o->callback(cb_Choice);
			choice = o;
		} // Fl_Choice* o
		{ Fl_Return_Button* o = new Fl_Return_Button(115, 85, 95, 25, _("OK"));
			o->callback((Fl_Callback*)cb_OK);
		} // Fl_Return_Button* o
		{ Fl_Button* o = new Fl_Button(235, 85, 95, 25, _("Cancel"));
			o->callback((Fl_Callback*)cb_Cancel);
		} // Fl_Button* o
		o->end();
	} // Fl_Double_Window* o

	char tmp[10] = "dial.1";
	tmp[5] += which;
	target = getSetting(tmp, NULL);

	// init
	dialurl->value("");
	choice->clear();
	u32 i;
	const u32 max = g->tabs.size();
	for (i = 0; i < max; i++) {
		const tab &cur = g->tabs[i];
		if (cur.state == TS_WEB) {
			choice->add(cur.title(), 0, 0, (void *) (uintptr_t) i);
		}
	}

	w->show();
	dialurl->take_focus();
}
