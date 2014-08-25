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
#include "lockicon.h"
#include "zoomer.h"

#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Box.H>

static Fl_PNG_Image *lockicon;
static Fl_Input *search;
static Fl_Box *total;

static void nextcb(Fl_Widget *, void *) {
	tab * const cur = &g->tabs[g->curtab];

	if (cur->state != TS_WEB
		|| search->size() < 2)
		return;

	cur->web->find(search->value());

	const u32 found = cur->web->countFound(search->value());
	char tmp[64];
	snprintf(tmp, 64, "%u matches", found);
	tmp[63] = '\0';

	total->copy_label(tmp);
}

static void prevcb(Fl_Widget *, void *) {
	if (g->tabs[g->curtab].state != TS_WEB
		|| search->size() < 2)
		return;

	g->tabs[g->curtab].web->find(search->value(), false, false);
}

static u32 jsw = 60;
static u32 cssw = 80;
static u32 imgw = 80;
static const u32 zoomerw = 100;

statusbar::statusbar(int x, int y, int w, int h): Fl_Group(x, y, w, h) {
	resizable(NULL);

	lockicon = new Fl_PNG_Image("lock.png", lock_png, sizeof(lock_png));

	search = new Fl_Input(x + h, y + 1, 150, h - 2);
	next = new Fl_Button(x + h + 150 + 3, y + 1, 100, h - 2,
				_("Find next"));
	prev = new Fl_Button(x + h + 150 + 3 + 100 + 3, y + 1, 100, h - 2,
				_("Find previous"));
	total = new Fl_Box(prev->x() + 100 + 3, y + 1, 100, h - 2);

	hidefind();

	// Calculate button widths
	fl_font(labelfont(), labelsize());
	int tmpw, tmph;
	const u32 pad = 10;

	tmpw = 0;
	fl_measure(_("js: auto"), tmpw, tmph);
	jsw = tmpw + pad;

	tmpw = 0;
	fl_measure(_("css: auto"), tmpw, tmph);
	cssw = tmpw + pad;

	tmpw = 0;
	fl_measure(_("img: auto"), tmpw, tmph);
	imgw = tmpw + pad;

	static const char * const tritip =
		_("Auto: decided based on per-site settings and global settings.\n"
		"On and off: override the settings for this tab.");

	js = new Fl_Button(1, y + 1, jsw, h - 2, _("js: auto"));
	css = new Fl_Button(1, y + 1, cssw, h - 2, _("css: auto"));
	img = new Fl_Button(1, y + 1, imgw, h - 2, _("img: auto"));

	js->tooltip(tritip);
	css->tooltip(tritip);
	img->tooltip(tritip);

	zoom = new zoomer(1, y + 1, zoomerw, h - 2);
	zoom->type(FL_HORIZONTAL);

	reposbuttons();

	end();

	next->callback(nextcb);
	prev->callback(prevcb);
	search->callback(nextcb);
	search->when(FL_WHEN_CHANGED | FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

	::search = search;
	::total = total;
}

void statusbar::draw() {
	u32 startx = x();
	const u32 endx = x() + w() - 1;

	// Button labels
	const tab * const cur = &g->tabs[g->curtab];

	switch (cur->css) {
		case TRI_AUTO:
			css->label(_("css: auto"));
		break;
		case TRI_OFF:
			css->label(_("css: off"));
		break;
		case TRI_ON:
			css->label(_("css: on"));
		break;
	}

	switch (cur->js) {
		case TRI_AUTO:
			js->label(_("js: auto"));
		break;
		case TRI_OFF:
			js->label(_("js: off"));
		break;
		case TRI_ON:
			js->label(_("js: on"));
		break;
	}

	switch (cur->img) {
		case TRI_AUTO:
			img->label(_("img: auto"));
		break;
		case TRI_OFF:
			img->label(_("img: off"));
		break;
		case TRI_ON:
			img->label(_("img: on"));
		break;
	}

	if (cur->state == TS_WEB) {
		css->activate();
		js->activate();
		img->activate();
		zoom->activate();
	} else {
		css->deactivate();
		js->deactivate();
		img->deactivate();
		zoom->deactivate();
	}

	if (damage() == FL_DAMAGE_CHILD) {
		draw_children();
		return;
	}

	// Background
	u32 i;
	const u32 max = h() - 2;
	u32 r1 = 209, g1 = 209, b1 = 209;
	u32 r2 = 120, g2 = 138, b2 = 147;
	for (i = 0; i <= max; i++) {
		const float pos = i / (float) max;

		fl_color(mix(r1, r2, pos),
			mix(g1, g2, pos),
			mix(b1, b2, pos));

		const u32 posy = y() + i;
		fl_line(startx, posy, endx, posy);
	}

	// Two border lines
	u32 posy = y() + max + 1;
	fl_color(79, 89, 100);
	fl_line(startx, posy, endx, posy);

	// SSL secure?
	if (cur->url && !strncmp(cur->url, "https://", 8)) {
		const u32 secw = h();

		u32 ix, iy;
		ix = startx + (secw - lockicon->w()) / 2;
		iy = y() + (h() - lockicon->h()) / 2;

		lockicon->draw(ix, iy);

		startx += secw;
	}

	// If search not visible, draw status text, if any
	if (!search->visible() && cur->state == TS_WEB && cur->web->statusbar()) {
		startx++;
		const char * const text = cur->web->statusbar();
		fl_font(FL_HELVETICA, 12);
		fl_color(FL_BLACK);
		fl_draw(text, startx, y(), w() - startx, h(), FL_ALIGN_LEFT);
	}

	draw_children();
}

int statusbar::handle(const int e) {

	if (e == FL_KEYDOWN && Fl::event_key() == FL_Escape)
		hidefind();

	return Fl_Group::handle(e);
}

void statusbar::startfind() {

	search->value("");
	total->label("");

	search->show();
	next->show();
	prev->show();
	total->show();

	search->take_focus();
}

void statusbar::findnext() {
	nextcb(NULL, NULL);
}

void statusbar::findprev() {
	prevcb(NULL, NULL);
}

void statusbar::hidefind() {

	search->hide();
	next->hide();
	prev->hide();
	total->hide();
}

void statusbar::resize(int x, int y, int w, int h) {
	Fl_Group::resize(x, y, w, h);

	reposbuttons();
}

void statusbar::reposbuttons() {

	zoom->position(x() + w() - zoomerw - 3, y() + 1);
	img->position(zoom->x() - imgw - 3, y() + 1);
	css->position(img->x() - cssw - 3, y() + 1);
	js->position(css->x() - jsw - 3, y() + 1);
}

void statusbar::refreshzoom() {
	const tab * const cur = &g->tabs[g->curtab];

	if (cur->state != TS_WEB) {
		zoom->value(1);
		return;
	}

	zoom->value(cur->web->getDouble(WK_SETTING_ZOOM));
}
