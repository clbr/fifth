/*
Copyright (C) 2014 Lauri Kasanen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>

#include "main.h"

static Fl_Browser *errlist=(Fl_Browser *)0;

static void cb_Close(Fl_Button *b, void*) {
	b->parent()->hide();
}

static void filtercb(Fl_Input *in, void *ptr) {

	histbuf *h = (histbuf *) ptr;
	const char *val = in->value();
	if (!val || strlen(val) < 2)
		return;

	const u32 max = h->size();
	errlist->clear();
	u32 i;
	for (i = 0; i < max; i++) {
		const char *tmp = h->getURL(i);
		if (strstr(tmp, val))
			errlist->add(tmp);
	}
}

static void clearcb(Fl_Widget *, void *ptr) {

	histbuf *h = (histbuf *) ptr;
	h->clear();
	errlist->clear();
}

void errorlog() {
	tab * const cur = &g->tabs[g->curtab];
	if (cur->state != TS_WEB)
		return;

	Fl_Double_Window* w;
	{ Fl_Double_Window* o = new Fl_Double_Window(905, 800, _("Error console"));
		w = o;
		{ errlist = new Fl_Browser(10, 10, 885, 745);
		} // Fl_Browser* errlist
		{ Fl_Input* o = new Fl_Input(50, 765, 370, 25, _("Filter:"));
			o->callback((Fl_Callback *) filtercb);
			o->when(FL_WHEN_CHANGED);
			o->user_data(cur->errors);
		} // Fl_Input* o
		{ Fl_Button* o = new Fl_Button(635, 765, 125, 25, _("Clear"));
			o->callback(clearcb);
			o->user_data(cur->errors);
		} // Fl_Button* o
		{ Fl_Button* o = new Fl_Button(770, 765, 125, 25, _("Close"));
			o->callback((Fl_Callback*)cb_Close);
		} // Fl_Button* o
		o->end();
	} // Fl_Double_Window* o

	errlist->clear();
	const u32 max = cur->errors->size();
	u32 i;
	for (i = 0; i < max; i++) {
		errlist->add(cur->errors->getURL(i));
	}

	w->show();
}
