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
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

void persitewindow();

static Fl_Double_Window *pswin=(Fl_Double_Window *)0;

static Fl_Input *pssite=(Fl_Input *)0;

static Fl_Choice *pscss=(Fl_Choice *)0;
static Fl_Choice *psjs=(Fl_Choice *)0;
static Fl_Choice *psimg=(Fl_Choice *)0;

static Fl_Input *psusercss=(Fl_Input *)0;
static Fl_Input *psuserjs=(Fl_Input *)0;

static Fl_Input *psspoofaccept=(Fl_Input *)0;
static Fl_Input *psspooflang=(Fl_Input *)0;
static Fl_Input *psspooftz=(Fl_Input *)0;
static Fl_Input *psspoofua=(Fl_Input *)0;

static Fl_Button *psok=(Fl_Button *)0;
static Fl_Button *pscancel=(Fl_Button *)0;

static void cb_pscancel(Fl_Button *b, void*) {
	b->window()->hide();
}

static void cb_psok(Fl_Widget *w, void*) {
	w->window()->hide();

	vector<setting> &vec = g->sitesettings[pssite->value()];
	setting s;
	bool force = false;

	// Apply settings

	// If the site changed, we need to apply everything
	if (pssite->changed())
		force = true;

	// Common tab
	if (pscss->changed() || force) {
		strcpy(s.name, "general.css");
		s.type = ST_U32;
		s.val.u = pscss->value();
		vec.push_back(s);
	}

	if (psjs->changed() || force) {
		strcpy(s.name, "general.javascript");
		s.type = ST_U32;
		s.val.u = psjs->value();
		vec.push_back(s);
	}

	if (psimg->changed() || force) {
		strcpy(s.name, "general.images");
		s.type = ST_U32;
		s.val.u = psimg->value();
		vec.push_back(s);
	}

	// Cookies tab

	// Spoof tab
	if (psspoofaccept->changed() || force) {
		strcpy(s.name, "spoof.accept");
		s.type = ST_CHAR;
		s.val.c = strdup(psspoofaccept->value());
		vec.push_back(s);
	}

	if (psspooflang->changed() || force) {
		strcpy(s.name, "spoof.language");
		s.type = ST_CHAR;
		s.val.c = strdup(psspooflang->value());
		vec.push_back(s);
	}

	if (psspooftz->changed() || force) {
		strcpy(s.name, "spoof.timezone");
		s.type = ST_U32;
		s.val.u = atoi(psspooftz->value());
		vec.push_back(s);
	}

	if (psspoofua->changed() || force) {
		strcpy(s.name, "spoof.useragent");
		s.type = ST_CHAR;
		s.val.c = strdup(psspoofua->value());
		vec.push_back(s);
	}

	saveConfig();
}

void persitewindow(const char * const site) {

	static const Fl_Menu_Item menu_bool[] = {
		{_("Off"), 0,	0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
		{_("On"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
		{0,0,0,0,0,0,0,0,0}
	};

	if (!pswin) {
		pswin = new Fl_Double_Window(535, 435, _("Per-site settings"));
		{ Fl_Tabs* o = new Fl_Tabs(0, 0, 570, 390);
			{ Fl_Group* o = new Fl_Group(0, 40, 570, 350, _("Common"));
				{ pssite = new Fl_Input(112, 65, 380, 25, _("Site:"));
				} // Fl_Input* pssite
				{ pscss = new Fl_Choice(112, 125, 380, 25, _("CSS:"));
					pscss->down_box(FL_BORDER_BOX);
					pscss->menu(menu_bool);
				} // Fl_Choice* pscss
				{ Fl_Box* o = new Fl_Box(85, 105, 400, 2);
					o->box(FL_THIN_DOWN_BOX);
				} // Fl_Box* o
				{ psjs = new Fl_Choice(112, 160, 380, 25, _("Javascript:"));
					psjs->down_box(FL_BORDER_BOX);
					psjs->menu(menu_bool);
				} // Fl_Choice* psjs
				{ psimg = new Fl_Choice(112, 195, 380, 25, _("Images:"));
					psimg->down_box(FL_BORDER_BOX);
					psimg->menu(menu_bool);
				} // Fl_Choice* psimg
				{ Fl_Box* o = new Fl_Box(85, 240, 400, 2);
					o->box(FL_THIN_DOWN_BOX);
				} // Fl_Box* o
				{ psusercss = new Fl_Input(112, 260, 380, 25, _("User CSS:"));
					psusercss->deactivate();
					psusercss->value(_("Not implemented yet"));
				} // Fl_Input* psusercss
				{ psuserjs = new Fl_Input(112, 300, 380, 25, _("User JS:"));
					psuserjs->deactivate();
					psuserjs->value(_("Not implemented yet"));
				} // Fl_Input* psuserjs
				o->end();
			} // Fl_Group* o
			{ Fl_Group* o = new Fl_Group(0, 40, 535, 350, _("Cookies"));
				o->hide();
				{ new Fl_Box(25, 65, 460, 115, _("Cookie control has not been implemented yet."));
				} // Fl_Box* o
				o->end();
			} // Fl_Group* o
			{ Fl_Group* o = new Fl_Group(0, 40, 535, 350, _("Spoofing"));
				o->hide();
				{ psspoofaccept = new Fl_Input(140, 65, 380, 25, _("Accept header:"));
				} // Fl_Input* psspoofaccept
				{ psspooflang = new Fl_Input(140, 100, 380, 25, _("Language header:"));
				} // Fl_Input* psspooflang
				{ psspooftz = new Fl_Input(140, 135, 380, 25, _("Timezone:"));
				} // Fl_Input* psspooftz
				{ psspoofua = new Fl_Input(140, 170, 380, 25, _("User-agent:"));
				} // Fl_Input* psspooftz
				o->end();
			} // Fl_Group* o
			o->end();
		} // Fl_Tabs* o
		{ psok = new Fl_Button(285, 400, 115, 25, _("OK"));
			psok->callback(cb_psok);
		} // Fl_Button* psok
		{ pscancel = new Fl_Button(410, 400, 115, 25, _("Cancel"));
			pscancel->callback((Fl_Callback*)cb_pscancel);
		} // Fl_Button* pscancel
		pswin->size_range(535, 435, 535, 435);
		pswin->end();
	} // Fl_Double_Window* pswin

	// Init values
	const setting *s;
	pssite->value(site);

	s = getSetting("general.css", site);
	pscss->value(!!s->val.u);

	s = getSetting("general.javascript", site);
	psjs->value(!!s->val.u);

	s = getSetting("general.images", site);
	psimg->value(!!s->val.u);

	// Cookies tab

	// Spoof tab
	s = getSetting("spoof.accept", site);
	psspoofaccept->value(s->val.c);
	psspoofaccept->position(0);

	s = getSetting("spoof.language", site);
	psspooflang->value(s->val.c);
	psspooflang->position(0);

	s = getSetting("spoof.timezone", site);
	char tmp[120];
	snprintf(tmp, 120, "%u", s->val.u);
	psspooftz->value(tmp);
	psspooftz->position(0);

	s = getSetting("spoof.useragent", site);
	psspoofua->value(s->val.c);
	psspoofua->position(0);

	pswin->show();
}
