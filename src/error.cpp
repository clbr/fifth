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
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>

#include "main.h"

static Fl_Select_Browser *errlist=(Fl_Select_Browser *)0;
static Fl_Double_Window* w = NULL;
static Fl_Input *filterin = NULL;
static Fl_Button *clearbtn = NULL;

static void cb_Close(Fl_Button *b, void*) {
	b->parent()->hide();
}

static void filtercb(Fl_Input *in, void *ptr) {

	histbuf *h = (histbuf *) ptr;
	const char *val = in->value();
	if (!val)
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

static void clipboardcb(Fl_Widget *, void *) {
	if (errlist->value()) {
		const char *text = errlist->text(errlist->value());
		Fl::copy(text, strlen(text));
	}
}

static void addsplit(const char in[]) {

	char tmp[1024];

	const char *cur = in, *end = strchrnul(in, '\n');

	do {
		const u32 len = end - cur;
		if (len >= 1024)
			return;

		memcpy(tmp, cur, len);
		tmp[len] = '\0';
		if (tmp[len - 1] == '\n') tmp[len - 1] = '\0';

		errlist->add(tmp);

		if (!*end)
			return;
		cur = end + 1;
		end = strchrnul(cur, '\n');

		if (!*end)
			return;
	} while (1);
}

void refresherr(const histbuf * const src) {
	if (!w || !w->shown())
		return;

	if (filterin->user_data() != src)
		return;

	errlist->clear();
	const u32 max = src->size();
	u32 i;
	for (i = 0; i < max; i++) {
		if (strchr(src->getURL(i), '\n'))
			addsplit(src->getURL(i));
		else
			errlist->add(src->getURL(i));
	}
}

void errorlog() {
	tab * const cur = &g->tabs[g->curtab];
	if (cur->state != TS_WEB)
		return;

	if (!w) {
		Fl_Double_Window* o = new Fl_Double_Window(905, 800, _("Error console"));
		w = o;
		w->resizable(w);
		{ errlist = new Fl_Select_Browser(10, 10, 885, 745);
			errlist->format_char(0);
			errlist->callback(clipboardcb);
			errlist->tooltip(_("Clicking on an item copies it to the clipboard."));
		} // Fl_Browser* errlist
		{ Fl_Input* o = new Fl_Input(50, 765, 370, 25, _("Filter:"));
			o->callback((Fl_Callback *) filtercb);
			o->when(FL_WHEN_CHANGED);
			filterin = o;
		} // Fl_Input* o
		{ Fl_Button* o = new Fl_Button(635, 765, 125, 25, _("Clear"));
			o->callback(clearcb);
			clearbtn = o;
		} // Fl_Button* o
		{ Fl_Button* o = new Fl_Button(770, 765, 125, 25, _("Close"));
			o->callback((Fl_Callback*)cb_Close);
		} // Fl_Button* o
		o->end();
	} // Fl_Double_Window* o

	filterin->user_data(cur->errors);
	clearbtn->user_data(cur->errors);

	errlist->clear();
	const u32 max = cur->errors->size();
	u32 i;
	for (i = 0; i < max; i++) {
		if (strchr(cur->errors->getURL(i), '\n'))
			addsplit(cur->errors->getURL(i));
		else
			errlist->add(cur->errors->getURL(i));
	}

	w->show();
}
