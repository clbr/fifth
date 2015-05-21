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
#include <FL/fl_ask.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>

static Fl_Double_Window *swin=(Fl_Double_Window *)0;

static Fl_Choice *sstartup=(Fl_Choice *)0;

Fl_Menu_Item menu_sstartup[] = {
 {_("Speed dial"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Homepage"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

static Fl_Input *shomepage=(Fl_Input *)0;

static Fl_Choice *scss=(Fl_Choice *)0;
static Fl_Choice *sjs=(Fl_Choice *)0;
static Fl_Choice *simg=(Fl_Choice *)0;
static Fl_Choice *slocalstor=(Fl_Choice *)0;

static Fl_Input *susercss=(Fl_Input *)0;
static Fl_Input *suserjs=(Fl_Input *)0;

static Fl_Input *sfill0=(Fl_Input *)0;
static Fl_Input *sfill1=(Fl_Input *)0;
static Fl_Input *sfill2=(Fl_Input *)0;
static Fl_Input *sfill3=(Fl_Input *)0;
static Fl_Input *sfill4=(Fl_Input *)0;
static Fl_Input *sfill5=(Fl_Input *)0;
static Fl_Input *sfill6=(Fl_Input *)0;
static Fl_Input *sfill7=(Fl_Input *)0;
static Fl_Input *sfill8=(Fl_Input *)0;
static Fl_Input *sfill9=(Fl_Input *)0;

static Fl_Input *sspoofaccept=(Fl_Input *)0;
static Fl_Input *sspooflang=(Fl_Input *)0;
static Fl_Input *sspooftz=(Fl_Input *)0;
static Fl_Input *sspoofua=(Fl_Input *)0;

static Fl_Input *sfont=(Fl_Input *)0;
static Fl_Input *sfixed=(Fl_Input *)0;

static Fl_Spinner *sfontsize=(Fl_Spinner *)0;
static Fl_Spinner *sfixedsize=(Fl_Spinner *)0;
static Fl_Spinner *sminfontsize=(Fl_Spinner *)0;

static Fl_Group *advfilter=(Fl_Group *)0;
static Fl_Browser *sblacklist=(Fl_Browser *)0;

static Fl_Group *advprog=(Fl_Group *)0;
static Fl_Input *sdlnotify=(Fl_Input *)0;
static Fl_Input *sdlopen=(Fl_Input *)0;
static Fl_Input *sstream=(Fl_Input *)0;

static Fl_Group *advhist=(Fl_Group *)0;
static Fl_Spinner *shistory=(Fl_Spinner *)0;

static void cb_Clear(Fl_Button*, void*) { // Clear history
	g->history->clear();
}

static bool blacklistchanged;

static Fl_Spinner *scache=(Fl_Spinner *)0;
static Fl_Group *advcookies=(Fl_Group *)0;

static Fl_Group *advhotkeys=(Fl_Group *)0;
static Fl_Browser *shotkeys=(Fl_Browser *)0;

class getwin: public Fl_Double_Window {
public:
	getwin(): Fl_Double_Window(400, 100), val(0) {
		label(_("Press the wanted keys..."));
	}

	int handle(int e) override {
		if (e == FL_KEYDOWN) {
			// Skip modifiers
			const int k = Fl::event_key();
			if (k == FL_Control_L ||
				k == FL_Control_R ||
				k == FL_Meta_L ||
				k == FL_Meta_R ||
				k == FL_Alt_L ||
				k == FL_Alt_R ||
				k == FL_Shift_L ||
				k == FL_Shift_R)
				return Fl_Double_Window::handle(e);

			keybinding key;
			key.key = k;
			key.ctrl = Fl::event_ctrl();
			key.alt = Fl::event_alt();
			key.shift = Fl::event_shift();

			val = keytou32(key);

			hide();
			return 1;
		}
		return Fl_Double_Window::handle(e);
	}

	u32 val;
};

static u32 getkey() {
	getwin *g = new getwin();
	g->show();
	while (g->shown())
		Fl::wait();

	const u32 val = g->val;
	delete g;
	return val;
}

static void cb_Edithotkey(Fl_Button*, void*) {
	const u32 cur = shotkeys->value();
	if (!cur) {
		fl_alert(_("No key selected"));
		return;
	}

	const char * const which = shotkeys->text(cur);
	char tmp[120];
	strncpy(tmp, which, 120);
	u32 i;
	for (i = 0; tmp[i]; i++)
		if (tmp[i] == '\t') {
			tmp[i] = '\0';
			break;
		}

	setting * const s = getSetting(tmp);
	const u32 val = getkey();
	s->val.u = val;

	const u32 key = menukey(tmp);
	char tmp2[120];
	snprintf(tmp2, 120, "%s\t%s", tmp,
		fl_shortcut_label(key));
	tmp2[119] = '\0';

	shotkeys->text(cur, tmp2);
	shotkeys->redraw();
}

static void cb_Clearhotkey(Fl_Button*, void*) {
	const u32 cur = shotkeys->value();
	if (!cur) {
		fl_alert(_("No key selected"));
		return;
	}

	const char * const which = shotkeys->text(cur);
	char tmp[120];
	strncpy(tmp, which, 120);
	u32 i;
	for (i = 0; tmp[i]; i++)
		if (tmp[i] == '\t') {
			tmp[i] = '\0';
			break;
		}

	setting * const s = getSetting(tmp);
	s->val.u = 0;

	shotkeys->text(cur, tmp);
	shotkeys->redraw();
}

static void saveblacklist() {
	const int fd = openat(g->profilefd, BLACKNAME, O_WRONLY | O_CREAT, 0600);
	if (fd < 0)
		return; // Maybe doesn't exist yet?
	FILE * const f = fdopen(fd, "w");
	if (!f) {
		close(fd);
		return;
	}

	u32 i;
	const u32 max = sblacklist->size();
	for (i = 0; i < max; i++) {
		fprintf(f, "%s\n", sblacklist->text(i + 1));
	}

	fclose(f);
}

static void cb_OK(Fl_Button *b, void*) {
	b->window()->hide();

	// Apply settings
	setting *s;

	// Common tab
	if (sstartup->changed()) {
		s = getSetting("general.startup");
		s->val.u = sstartup->value();
	}

	if (shomepage->changed()) {
		s = getSetting("general.homepage");
		s->val.c = strdup(shomepage->value());
	}

	if (scss->changed()) {
		s = getSetting("general.css");
		s->val.u = scss->value();
	}

	if (sjs->changed()) {
		s = getSetting("general.javascript");
		s->val.u = sjs->value();
	}

	if (simg->changed()) {
		s = getSetting("general.images");
		s->val.u = simg->value();
	}

	if (slocalstor->changed()) {
		s = getSetting("general.localstorage");
		s->val.u = slocalstor->value();
	}

	if (susercss->changed()) {
		s = getSetting("user.css");
		s->val.c = strdup(susercss->value());
	}

	// Autocomplete tab
	static Fl_Input *autos[10] = {
		sfill0,
		sfill1,
		sfill2,
		sfill3,
		sfill4,
		sfill5,
		sfill6,
		sfill7,
		sfill8,
		sfill9
	};
	u32 i;
	for (i = 0; i < 10; i++) {
		if (!autos[i]->changed())
			continue;

		char tgt[] = "autocomplete.0";
		tgt[13] += i;
		s = getSetting(tgt);
		s->val.c = strdup(autos[i]->value());
	}

	// Spoofing tab
	if (sspoofaccept->changed()) {
		s = getSetting("spoof.accept");
		s->val.c = strdup(sspoofaccept->value());
	}

	if (sspooflang->changed()) {
		s = getSetting("spoof.language");
		s->val.c = strdup(sspooflang->value());
	}

	if (sspooftz->changed()) {
		s = getSetting("spoof.timezone");
		s->val.u = atoi(sspooftz->value());
	}

	if (sspoofua->changed()) {
		s = getSetting("spoof.useragent");
		s->val.c = strdup(sspoofua->value());
	}

	// Looks tab
	if (sfont->changed()) {
		s = getSetting("fonts.default");
		s->val.c = strdup(sfont->value());
	}

	if (sfixed->changed()) {
		s = getSetting("fonts.fixed");
		s->val.c = strdup(sfixed->value());
	}

	if (sfontsize->changed()) {
		s = getSetting("fonts.size");
		s->val.u = sfontsize->value();
	}

	if (sfixedsize->changed()) {
		s = getSetting("fonts.fixedsize");
		s->val.u = sfixedsize->value();
	}

	if (sminfontsize->changed()) {
		s = getSetting("fonts.minsize");
		s->val.u = sminfontsize->value();
	}

	// Advanced filter tab
	if (blacklistchanged)
		saveblacklist();

	// Advanced programs tab
	if (sdlnotify->changed()) {
		s = getSetting("exec.downloadnotify");
		s->val.c = strdup(sdlnotify->value());
	}

	if (sdlopen->changed()) {
		s = getSetting("exec.open");
		s->val.c = strdup(sdlopen->value());
	}

	if (sstream->changed()) {
		s = getSetting("exec.stream");
		s->val.c = strdup(sstream->value());
	}

	// Advanced history tab
	if (shistory->changed()) {
		s = getSetting("history.size");
		s->val.u = shistory->value();
	}

	if (scache->changed()) {
		s = getSetting("cache.disk");
		s->val.u = scache->value() * 1024 * 1024;
	}

	// Advanced cookies tab

	// Advanced hotkeys tab - nothing needed

	saveConfig();
}

static void cb_Cancel(Fl_Button *b, void*) {
	b->window()->hide();
}

static void advancedcb(Fl_Widget *w, void*) {
	Fl_Browser *b = (Fl_Browser *) w;

	advfilter->hide();
	advprog->hide();
	advhist->hide();
	advcookies->hide();
	advhotkeys->hide();

	switch (b->value()) {
		case 1:
			advfilter->show();
		break;
		case 2:
			advprog->show();
		break;
		case 3:
			advhist->show();
		break;
		case 4:
			advcookies->show();
		break;
		case 5:
			advhotkeys->show();
		break;
	}
}

static void loadblacklist() {
	sblacklist->clear();
	blacklistchanged = false;

	const int fd = openat(g->profilefd, BLACKNAME, O_RDONLY);
	if (fd < 0)
		return; // Maybe doesn't exist yet?
	FILE * const f = fdopen(fd, "r");
	if (!f) {
		close(fd);
		return;
	}

	char buf[600];
	while (fgets(buf, 600, f)) {
		nukenewline(buf);
		sblacklist->add(buf);
	}

	fclose(f);
}

static void cssfile(Fl_Widget *, void *) {

	const char *home = getenv("HOME");
	if (!home)
		home = "/home";

	const char *picked = fl_file_chooser(_("Select CSS file"), "*.css", home, 0);
	if (!picked)
		return;

	susercss->value(picked);
	susercss->set_changed();
}

static void addentry(Fl_Widget *, void *) {
	const char *msg = fl_input(_("Add a new blacklist entry"), "");
	if (msg)
		sblacklist->add(msg);
	sblacklist->redraw();
	blacklistchanged = true;
}

static void editentry(Fl_Widget *, void *) {
	const u32 cur = sblacklist->value();
	if (!cur) {
		fl_alert(_("No entry selected"));
		return;
	}

	const char *msg = fl_input(_("Edit blacklist entry"), sblacklist->text(cur));
	if (msg)
		sblacklist->text(cur, msg);
	sblacklist->redraw();
	blacklistchanged = true;
}

static void uafunc(Fl_Widget *, void *ptr) {
	const ua_t agent = (ua_t) (uintptr_t) ptr;
	sspoofua->value(uas[agent]);
	sspoofua->set_changed();
}

void settingswindow() {

	static const Fl_Menu_Item menu_bool[] = {
		{_("Off"), 0,	0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
		{_("On"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
		{0,0,0,0,0,0,0,0,0}
	};

	if (!swin) {
		swin = new Fl_Double_Window(615, 470, _("Settings"));
		{ Fl_Tabs* o = new Fl_Tabs(0, 0, 620, 425);
			{ Fl_Group* o = new Fl_Group(0, 40, 615, 380, _("Common"));
				{ sstartup = new Fl_Choice(115, 80, 380, 25, _("Startup:"));
					sstartup->down_box(FL_BORDER_BOX);
					sstartup->menu(menu_sstartup);
					sstartup->when(0);
				} // Fl_Choice* sstartup
				{ shomepage = new Fl_Input(115, 115, 380, 25, _("Home page:"));
					shomepage->when(0);
				} // Fl_Input* shomepage
				{ scss = new Fl_Choice(117, 180, 380, 25, _("CSS:"));
					scss->down_box(FL_BORDER_BOX);
					scss->menu(menu_bool);
					scss->when(0);
				} // Fl_Choice* scss
				{ Fl_Box* o = new Fl_Box(107, 160, 400, 2);
					o->box(FL_THIN_DOWN_BOX);
				} // Fl_Box* o
				{ sjs = new Fl_Choice(117, 215, 380, 25, _("Javascript:"));
					sjs->down_box(FL_BORDER_BOX);
					sjs->menu(menu_bool);
					sjs->when(0);
				} // Fl_Choice* sjs
				{ simg = new Fl_Choice(117, 250, 380, 25, _("Images:"));
					simg->down_box(FL_BORDER_BOX);
					simg->menu(menu_bool);
					simg->when(0);
				} // Fl_Choice* simg
				{ slocalstor = new Fl_Choice(117, 285, 380, 25, _("LocalStorage:"));
					slocalstor->down_box(FL_BORDER_BOX);
					slocalstor->menu(menu_bool);
					slocalstor->when(0);
				} // Fl_Choice* slocalstor
				{ Fl_Box* o = new Fl_Box(107, 330, 400, 2);
					o->box(FL_THIN_DOWN_BOX);
				} // Fl_Box* o
				{ susercss = new Fl_Input(117, 350, 348, 25, _("User CSS:"));
					susercss->tooltip(_("Only one user stylesheet can be "
						"in effect at once. If there is no site-specific "
						"one, the global one is used."));
					susercss->when(0);
				} // Fl_Input* susercss
				{ Fl_Button *o = new Fl_Button(470, 350, 25, 25);
					o->image(Fl_Shared_Image::get("folder.png"));
					o->callback(cssfile);
				}
				{ suserjs = new Fl_Input(117, 390, 380, 25, _("User JS:"));
					suserjs->deactivate();
					suserjs->value(_("Not implemented yet"));
					suserjs->when(0);
				} // Fl_Input* suserjs
				o->end();
			} // Fl_Group* o
			{ Fl_Group* o = new Fl_Group(0, 40, 615, 380, _("Autocomplete"));
				o->hide();
				{ Fl_Box* o = new Fl_Box(32, 65, 550, 75, _("Fifth will never store what you fill in forms, as that would be a security ri\
sk. The only options suggested for autocomplete are those you explicitly fill \
in here. Note that autocomplete is not yet implemented."));
					o->align(132|FL_ALIGN_INSIDE);
				} // Fl_Box* o
				{ sfill0 = new Fl_Input(30, 160, 250, 25);
					sfill0->when(0);
				} // Fl_Input* sfill0
				{ sfill1 = new Fl_Input(30, 198, 250, 25);
					sfill1->when(0);
				} // Fl_Input* sfill1
				{ sfill2 = new Fl_Input(30, 237, 250, 25);
					sfill2->when(0);
				} // Fl_Input* sfill2
				{ sfill3 = new Fl_Input(30, 276, 250, 25);
					sfill3->when(0);
				} // Fl_Input* sfill3
				{ sfill4 = new Fl_Input(30, 315, 250, 25);
					sfill4->when(0);
				} // Fl_Input* sfill4
				{ sfill5 = new Fl_Input(325, 160, 250, 25);
					sfill5->when(0);
				} // Fl_Input* sfill5
				{ sfill6 = new Fl_Input(325, 198, 250, 25);
					sfill6->when(0);
				} // Fl_Input* sfill6
				{ sfill7 = new Fl_Input(325, 237, 250, 25);
					sfill7->when(0);
				} // Fl_Input* sfill7
				{ sfill8 = new Fl_Input(325, 276, 250, 25);
					sfill8->when(0);
				} // Fl_Input* sfill8
				{ sfill9 = new Fl_Input(325, 315, 250, 25);
					sfill9->when(0);
				} // Fl_Input* sfill9
				o->end();
			} // Fl_Group* o
			{ Fl_Group* o = new Fl_Group(0, 40, 615, 380, _("Spoofing"));
				o->hide();
				{ sspoofaccept = new Fl_Input(170, 65, 420, 25, _("Accept header:"));
					sspoofaccept->when(0);
				} // Fl_Input* sspoofaccept
				{ sspooflang = new Fl_Input(170, 100, 420, 25, _("Language header:"));
					sspooflang->when(0);
				} // Fl_Input* sspooflang
				{ sspooftz = new Fl_Input(170, 135, 420, 25, _("Timezone:"));
					sspooftz->when(0);
				} // Fl_Input* sspooftz
				{ sspoofua = new Fl_Input(170, 170, 420, 25, _("User-agent:"));
					sspoofua->when(0);
				} // Fl_Input* sspoofua
				{ Fl_Button *b = new Fl_Button(170, 205, 70, 25, _("Default"));
					b->callback(uafunc);
					b->user_data((void *) UA_DEFAULT);
				}
				{ Fl_Button *b = new Fl_Button(257, 205, 70, 25, _("IE"));
					b->callback(uafunc);
					b->user_data((void *) UA_IE);
				}
				{ Fl_Button *b = new Fl_Button(345, 205, 70, 25, _("Chrome"));
					b->callback(uafunc);
					b->user_data((void *) UA_CHROME);
				}
				{ Fl_Button *b = new Fl_Button(432, 205, 70, 25, _("Safari"));
					b->callback(uafunc);
					b->user_data((void *) UA_SAFARI);
				}
				{ Fl_Button *b = new Fl_Button(520, 205, 70, 25, _("Opera"));
					b->callback(uafunc);
					b->user_data((void *) UA_OPERA);
				}
				o->end();
			} // Fl_Group* o
			{ Fl_Group* o = new Fl_Group(0, 40, 615, 380, _("Looks"));
				o->hide();
				{ sfont = new Fl_Input(157, 65, 380, 25, _("Default font:"));
					sfont->when(0);
				} // Fl_Input* sfont
				{ sfixed = new Fl_Input(155, 101, 380, 25, _("Fixed font:"));
					sfixed->when(0);
				} // Fl_Input* sfixed
				{ sfontsize = new Fl_Spinner(155, 137, 380, 25, _("Default font size:"));
					sfontsize->minimum(8);
					sfontsize->maximum(128);
					sfontsize->when(0);
				} // Fl_Spinner* sfontsize
				{ sfixedsize = new Fl_Spinner(155, 173, 380, 25, _("Default fixed size:"));
					sfixedsize->minimum(8);
					sfixedsize->maximum(128);
					sfixedsize->when(0);
				} // Fl_Spinner* sfixedsize
				{ sminfontsize = new Fl_Spinner(155, 210, 380, 25, _("Minimum font size:"));
					sminfontsize->minimum(8);
					sminfontsize->maximum(128);
					sminfontsize->when(0);
				} // Fl_Spinner* sminfontsize
				o->end();
			} // Fl_Group* o
			{ Fl_Group* o = new Fl_Group(0, 40, 620, 385, _("Advanced"));
				o->hide();
				{ Fl_Browser* o = new Fl_Browser(10, 50, 140, 365);
					o->type(2);
					o->add(_("Filter"));
					o->add(_("Programs"));
					o->add(_("History"));
					o->add(_("Cookies"));
					o->add(_("Hotkeys"));
					o->callback(advancedcb);
				} // Fl_Browser* o
				{ advfilter = new Fl_Group(155, 45, 455, 375);
					advfilter->hide();
					{ Fl_Button *o = new Fl_Button(160, 55, 195, 30, _("Edit filtering entry"));
						o->callback(editentry);
					} // Fl_Button* o
					{ Fl_Button *o = new Fl_Button(365, 55, 195, 30, _("Add new entry"));
						o->callback(addentry);
					} // Fl_Button* o
					{ sblacklist = new Fl_Browser(160, 95, 445, 320);
						sblacklist->type(2);
					} // Fl_Browser* sblacklist
					advfilter->end();
				} // Fl_Group* advfilter
				{ advprog = new Fl_Group(155, 45, 455, 375);
					advprog->hide();
					{ sdlnotify = new Fl_Input(305, 70, 300, 25, _("Download notification program:"));
						sdlnotify->align(132);
						sdlnotify->when(0);
					} // Fl_Input* sdlnotify
					{ sdlopen = new Fl_Input(305, 110, 300, 25, _("Open downloads with:"));
						sdlopen->align(132);
						sdlopen->when(0);
					} // Fl_Input* sdlopen
					{ sstream = new Fl_Input(305, 150, 300, 25, _("Stream with:"));
						sstream->align(132);
						sstream->when(0);
					} // Fl_Input* sstream
					advprog->end();
				} // Fl_Group* advprog
				{ advhist = new Fl_Group(155, 45, 455, 375);
					advhist->hide();
					{ shistory = new Fl_Spinner(395, 70, 180, 25, _("Remembered addresses:"));
						shistory->minimum(100);
						shistory->maximum(10000);
						shistory->step(100);
						shistory->value(100);
						shistory->when(0);
					} // Fl_Spinner* shistory
					{ Fl_Button* o = new Fl_Button(395, 105, 180, 25, _("Clear history"));
						o->callback((Fl_Callback*)cb_Clear);
					} // Fl_Button* o
					{ scache = new Fl_Spinner(395, 175, 180, 25, _("Cache size (megabytes):"));
						scache->maximum(1000);
						scache->value(20);
						scache->when(0);
					} // Fl_Spinner* scache
					advhist->end();
				} // Fl_Group* advhist
				{ advcookies = new Fl_Group(155, 45, 455, 375);
					advcookies->hide();
					{ new Fl_Box(180, 70, 400, 90, _("Cookie control is not yet implemented."));
					} // Fl_Box* o
					advcookies->end();
				} // Fl_Group* advcookies
				{ advhotkeys = new Fl_Group(155, 45, 455, 375);
					advhotkeys->hide();
					{ shotkeys = new Fl_Browser(160, 50, 445, 330);
						shotkeys->type(2);
						shotkeys->column_char('\t');
						static const int wid[] = {220, 220, 0};
						shotkeys->column_widths(wid);
					} // Fl_Browser* shotkeys
					{ Fl_Button* o = new Fl_Button(290, 390, 150, 25, _("Edit hotkey"));
						o->callback((Fl_Callback*)cb_Edithotkey);
					} // Fl_Button* o
					{ Fl_Button* o = new Fl_Button(450, 390, 150, 25, _("Clear hotkey"));
						o->callback((Fl_Callback*)cb_Clearhotkey);
					} // Fl_Button* o
					advhotkeys->end();
				} // Fl_Group* advhotkeys
				o->end();
			} // Fl_Group* o
			o->end();
		} // Fl_Tabs* o
		{ Fl_Button* o = new Fl_Button(365, 435, 115, 25, _("OK"));
			o->callback((Fl_Callback*)cb_OK);
		} // Fl_Button* o
		{ Fl_Button* o = new Fl_Button(490, 435, 115, 25, _("Cancel"));
			o->callback((Fl_Callback*)cb_Cancel);
		} // Fl_Button* o
		{ new Fl_Box(10, 435, 340, 25, _("Some settings may require a restart."));
		}
		swin->size_range(615, 470, 615, 470);
		swin->end();
	} // Fl_Double_Window* swin

	// Init values
	const setting *s;

	// Common tab
	s = getSetting("general.startup");
	sstartup->value(s->val.u);

	s = getSetting("general.homepage");
	shomepage->value(s->val.c);

	s = getSetting("general.css");
	scss->value(s->val.u);

	s = getSetting("general.javascript");
	sjs->value(s->val.u);

	s = getSetting("general.images");
	simg->value(s->val.u);

	s = getSetting("general.localstorage");
	slocalstor->value(s->val.u);

	s = getSetting("user.css");
	susercss->value(s->val.c);

	// Autocomplete tab
	static Fl_Input *autos[10] = {
		sfill0,
		sfill1,
		sfill2,
		sfill3,
		sfill4,
		sfill5,
		sfill6,
		sfill7,
		sfill8,
		sfill9
	};
	u32 i;
	for (i = 0; i < 10; i++) {
		char tgt[] = "autocomplete.0";
		tgt[13] += i;
		s = getSetting(tgt);
		autos[i]->value(s->val.c);
	}

	// Spoofing tab
	s = getSetting("spoof.accept");
	sspoofaccept->value(s->val.c);
	sspoofaccept->position(0);

	s = getSetting("spoof.language");
	sspooflang->value(s->val.c);
	sspooflang->position(0);

	s = getSetting("spoof.timezone");
	char tmp[120];
	snprintf(tmp, 120, "%u", s->val.u);
	sspooftz->value(tmp);
	sspooftz->position(0);

	s = getSetting("spoof.useragent");
	sspoofua->value(s->val.c);
	sspoofua->position(0);

	// Looks tab
	s = getSetting("fonts.default");
	sfont->value(s->val.c);

	s = getSetting("fonts.fixed");
	sfixed->value(s->val.c);

	s = getSetting("fonts.size");
	sfontsize->value(s->val.u);

	s = getSetting("fonts.fixedsize");
	sfixedsize->value(s->val.u);

	s = getSetting("fonts.minsize");
	sminfontsize->value(s->val.u);

	// Advanced filter tab
	loadblacklist();

	// Advanced programs tab
	s = getSetting("exec.downloadnotify");
	sdlnotify->value(s->val.c);

	s = getSetting("exec.open");
	sdlopen->value(s->val.c);

	s = getSetting("exec.stream");
	sstream->value(s->val.c);

	// Advanced history tab
	s = getSetting("history.size");
	shistory->value(s->val.u);

	s = getSetting("cache.disk");
	scache->value(s->val.u / 1024 / 1024);

	// Advanced cookies tab

	// Advanced hotkeys tab
	shotkeys->clear();
	for (i = 0; i < numDefaults; i++) {
		const int ret = strncmp(defaultSettings[i].name, "keys.", 5);
		if (ret < 0)
			continue;
		if (ret > 0)
			break;

		const u32 key = menukey(defaultSettings[i].name);
		char tmp[120];
		snprintf(tmp, 120, "%s\t%s", defaultSettings[i].name,
			fl_shortcut_label(key));
		tmp[119] = '\0';

		shotkeys->add(tmp);
	}

	swin->show();
}
