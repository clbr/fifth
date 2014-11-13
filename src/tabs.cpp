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
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Tooltip.H>

static vector<u16> longorder;
static u16 longpress;
static bool firstpress;

extern Fl_PNG_Image *ddglogo, *googlelogo;

tabbar::tabbar(int x, int y, int w, int h): Fl_Widget(x, y, w, h),
		mousex(0), mousein(false), dragging(false), srctab(0) {
	labelsize(12);
}

static u32 calctabw(u32 *outmax, const u32 w) {

	const u32 defw = 150, minw = 50;
	u32 max = g->tabs.size();
	u32 tabw = defw;

	if (tabw * max >= w) {
		tabw = w / max;
		while (tabw < minw) {
			max--;
			tabw = w / max;
		}
	}

	*outmax = max;
	return tabw;
}

void tabbar::draw() {
	const u32 startx = x();
	const u32 endx = x() + w() - 1;

	const u32 gradarea = h() - 2;
	u32 i, j;

	// bg
	u32 r1 = 63, g1 = 72, b1 = 81;
	u32 r2 = 38, g2 = 48, b2 = 59;
	for (i = 0; i <= gradarea; i++) {
		const float pos = i / (float) gradarea;

		fl_color(mix(r1, r2, pos),
			mix(g1, g2, pos),
			mix(b1, b2, pos));

		const u32 posy = y() + 1 + i;
		fl_line(startx, posy, endx, posy);
	}

	// Borders
	fl_color(37, 46, 52);
	fl_line(startx, y(), endx, y());
	fl_line(startx, y() + h() - 1, endx, y() + h() - 1);

	// Tabs
	u32 max;
	u32 tabw = calctabw(&max, w());

	bool highborder = false;
	if ((mousex - x()) % tabw > tabw / 2)
		highborder = true;

	u32 posx = x();
	const u32 lowy = y();
	const u32 highy = y() + h() - 1;
	for (i = 0; i < max; i++) {
		bool hover = false;
		// Borders
		fl_color(FL_BLACK);
		if (!i)
			fl_line(posx, lowy, posx, highy);
		fl_line(posx + tabw, lowy, posx + tabw, highy);

		// Bg
		r1 = 83, g1 = 92, b1 = 101;
		r2 = 58, g2 = 68, b2 = 79;

		if (i == g->curtab) {
			r1 = 100, g1 = 160, b1 = 191;
			r2 = 42, g2 = 100, b2 = 125;
		} else if ((mousein && mousex >= posx && mousex <= (posx + tabw - 1)) ||
				(!firstpress && longorder.size() && longorder[longpress] == i)) {
			r1 = 150, g1 = 150, b1 = 191;
			r2 = 100, g2 = 100, b2 = 155;
			hover = true;
		}

		if (dragging && mousein && mousex >= posx && mousex <= (posx + tabw - 1)) {
			fl_color(FL_WHITE);
			if (!highborder)
				fl_line(posx, lowy, posx, highy);
			else
				fl_line(posx + tabw, lowy, posx + tabw, highy);
		}

		for (j = 0; j <= gradarea; j++) {
			const float pos = j / (float) gradarea;

			fl_color(mix(r1, r2, pos),
				mix(g1, g2, pos),
				mix(b1, b2, pos));

			const u32 posy = y() + 1 + j;
			fl_line(posx + 1, posy, posx + tabw - 1, posy);
		}

		// Icon
		Fl_Image *img = g->tabs[i].icon;
		if (!img)
			img = Fl_Shared_Image::get("newtab.png");

		const u32 imgy = y() + (h() - 16) / 2;
		img->draw(posx + 4, imgy, 16, 16);

		// Text
		fl_color(200, 200, 200);
		if (i == g->curtab)
			fl_color(FL_WHITE);
		else if (hover)
			fl_color(230, 230, 230);
		fl_font(labelfont(), labelsize());

		char tmp[80];
		memset(tmp, 0, 80);
		strncpy(tmp, g->tabs[i].title(), 79);

		int textw = 0, texth = 0;
		fl_measure(tmp, textw, texth, 0);

		const u32 textarea = tabw - 2 - 8 - 16 - 4;
		while ((u32) textw >= textarea) {
			// Shorten it until it fits
			const u32 len = strlen(tmp) - 1;
			tmp[len] = '\0';
			tmp[len - 1] = '.';
			tmp[len - 2] = '.';
			tmp[len - 3] = '.';

			textw = texth = 0;
			fl_measure(tmp, textw, texth, 0);
		}

		const u32 texty = y() - fl_descent() + fl_height() + (h() - fl_height())/2;
		fl_draw(tmp, posx + 2 + 8 + 16, texty);

		posx += tabw;
	}
}

int tabbar::handle(const int e) {

	bool ontab = false;
	u32 which = 0;
	bool leftcloser = false;

	// Over a tab?
	if (mousein) {
		u32 max;
		const u32 tabw = calctabw(&max, w());

		if (mousex < x() + max * tabw && mousex >= (u32) x()) {
			ontab = true;
			u32 tmp = mousex - x();

			const u32 whichmod = tmp % tabw;
			if (whichmod < tabw / 2)
				leftcloser = true;

			tmp /= tabw;
			which = tmp;
		}
	}

	switch (e) {
		case FL_ENTER:
			mousein = true;
			return 1;
		case FL_LEAVE:
			mousein = false;
			redraw();
			return 1;
		case FL_DRAG:
			if (!dragging && ontab && Fl::event_state(FL_BUTTON1)) {
				srctab = which;
				dragging = true;
			}
			// Fallthrough
		case FL_MOVE:
			mousex = Fl::event_x();
			redraw();

			if (ontab) {
				if (!tooltip() || strcmp(tooltip(), g->tabs[which].title())) {
					copy_tooltip(g->tabs[which].title());
					Fl_Tooltip::enter(0);
					Fl_Tooltip::enter(this);
				}
			} else {
				if (!tooltip() || strcmp(tooltip(), "")) {
					tooltip("");
					Fl_Tooltip::enter(0);
					Fl_Tooltip::enter(this);
				}
			}

			return 1;
		case FL_PUSH:
			// Double click on empty?
			if (Fl::event_clicks() && Fl::event_button() == FL_LEFT_MOUSE &&
				!ontab) {
				newtab();
				return 1;
			}

			return 1;
		case FL_RELEASE:
			if (dragging) {
				const tab tmp = g->tabs[srctab];

				if (ontab) {
					u32 newpos = which;

					// No drag?
					if ((srctab == newpos - 1 && leftcloser) ||
						(srctab == newpos + 1 && !leftcloser) ||
						srctab == which)
						goto dragdone;

					if (!leftcloser && newpos < srctab)
						newpos++;
					else if (leftcloser && newpos > srctab)
						newpos--;
					g->tabs.erase(g->tabs.begin() + srctab);

					if (newpos < g->tabs.size())
						g->tabs.insert(g->tabs.begin() + newpos, tmp);
					else
						g->tabs.push_back(tmp);

					if (g->curtab == srctab)
						g->curtab = newpos;
					else if (g->curtab > srctab && g->curtab <= newpos)
						g->curtab--;
					else if (g->curtab < srctab && g->curtab >= newpos)
						g->curtab++;

					if (g->curtab >= g->tabs.size())
						g->curtab = g->tabs.size() - 1;
				} else {
					g->tabs.erase(g->tabs.begin() + srctab);
					g->tabs.push_back(tmp);

					if (g->curtab > srctab)
						g->curtab--;
					else if (g->curtab == srctab)
						g->curtab = g->tabs.size() - 1;
				}
			} else if (ontab) {
				if (Fl::event_button() == FL_LEFT_MOUSE) {
					activatetab(which);
				} else if (Fl::event_button() == FL_MIDDLE_MOUSE) {
					activatetab(which);
					closetab();
				}
			}

			dragdone:
			dragging = false;
			return 1;
	}

	return Fl_Widget::handle(e);
}

tab::tab(): state(TS_WEB), engine(TSE_DDG), web(NULL), lastactive(msec()),
		icon(NULL), url(NULL), search(NULL), sslsite(NULL),
		progress(100), css(TRI_AUTO), js(TRI_AUTO), img(TRI_AUTO) {

	errors = new histbuf(1000);
}

tab::~tab() {
}

const char *tab::title() const {

	const char *tmp = NULL;

	switch (state) {
		case TS_WEB:
			tmp = web->title();
			return tmp ? tmp : "(no title)";
		break;
		case TS_DOWNLOAD:
			return _("Downloads");
		break;
		case TS_SPEEDDIAL:
			return _("Speed dial");
		break;
		case TS_SSLERR:
			return _("Security risk");
		break;
		case TS_BOOKMARKS:
			return _("Edit bookmarks");
		break;
		case TS_COUNT:
			die("Tab corruption\n");
	}

	die("Tab corruption\n");
}

static void windowtitle() {
	static char *oldtitle = NULL;
	static char *store = NULL;

	if (!oldtitle || strcmp(oldtitle, g->tabs[g->curtab].title())) {
		free(oldtitle);
		free(store);
		asprintf(&store, "%s - Fifth", g->tabs[g->curtab].title());
		g->w->label(store);
		oldtitle = strdup(g->tabs[g->curtab].title());
	}
}

static void searchenginestate() {
	// Check search engine state
	switch (g->tabs[g->curtab].engine) {
		case TSE_DDG:
			g->url->search->input().placeholder("DuckDuckGo");
			g->url->search->input().image(ddglogo);
		break;
		case TSE_GOOGLE:
			g->url->search->input().placeholder("Google");
			g->url->search->input().image(googlelogo);
		break;
		case TSE_COUNT:
			die("Search engine corruption\n");
	}
}

static void urlbarstate(bool curcheck = false) {
	const tab * const cur = &g->tabs[g->curtab];
	if (curcheck && (cur->state != TS_WEB || !cur->web))
		return;

	if (cur->url) {
		g->url->url->input().value(cur->url);
	} else {
		g->url->url->input().static_value("");
	}
	g->url->url->input().position(0);

	if (cur->search) {
		g->url->search->input().value(cur->search);
	} else {
		g->url->search->input().static_value("");
	}
	g->url->search->input().position(0);

	g->url->url->input().image(cur->icon);
	urlbuttonstate();
	g->status->redraw(); // maybe SSL state changed
}

static void saveurlbar() {
	tab * const cur = &g->tabs[g->curtab];

	free((char *) cur->url);
	free((char *) cur->search);
	cur->url = NULL;
	cur->search = NULL;

	const char * const urlval = g->url->url->input().value();
	const char * const searchval = g->url->search->input().value();

	if (strlen(urlval) > 1 && !allspace(urlval))
		cur->url = strdup(urlval);
	if (strlen(searchval) > 1 && !allspace(searchval))
		cur->search = strdup(searchval);
}

static void titlecb() {
	g->tabwidget->redraw();

	windowtitle();

	// Titles are often changed with urls. Sync urls here.
	u32 i;
	const u32 max = g->tabs.size();

	for (i = 0; i < max; i++) {
		tab * const cur = &g->tabs[i];

		free((char *) cur->url);
		cur->url = NULL;
		if (cur->web && cur->web->url())
			cur->url = strdup(cur->web->url());
	}

	urlbarstate(true);
}

static void donecheck(void *) {
	const tab * const cur = &g->tabs[g->curtab];
	if (cur->state != TS_WEB)
		return;

	if (cur->web->isLoading())
		g->url->refreshstate(false);
	else
		g->url->refreshstate(true);
	g->url->redraw();
}

static void stopcb(webview * const view) {
	const tab * const cur = &g->tabs[g->curtab];
	if (cur->state != TS_WEB || cur->web != view)
		return;

	const bool loading = cur->web->isLoading();

	if (loading)
		g->url->refreshstate(false);
	else
		g->url->refreshstate(true);

	g->url->redraw();

	// Some pages have slower sub-resources. Wait a second
	if (loading)
		Fl::add_timeout(1, donecheck);
}

static void faviconcb(webview * const view) {
	tab * const cur = findtab(view);
	if (!cur || cur->state != TS_WEB)
		return;

	if (cur->icon)
		delete cur->icon;
	cur->icon = wk_get_favicon(cur->url);

	g->url->url->input().image(NULL);
	if (!cur->icon)
		return;

	// It may be in a wrong resolution - resize in that case
	if (cur->icon->w() != 16 || cur->icon->h() != 16) {
		Fl_RGB_Image *old = cur->icon;
		cur->icon = (Fl_RGB_Image *) old->copy(16, 16);
		delete old;
	}

	if (g->tabs[g->curtab].web == view) {
		g->url->url->input().image(cur->icon);
		g->url->redraw();
	}
	g->tabwidget->redraw();
}

static void progresscb(webview * const view, float val) {
	tab * const cur = findtab(view);
	if (!cur || cur->state != TS_WEB)
		return;

	val *= 100;

	cur->progress = val;

	if (cur == &g->tabs[g->curtab])
		g->url->redraw();

	// If this is a new one, reset the favicon
	if (val < 15) {
		if (cur->icon)
			delete cur->icon;
		cur->icon = NULL;
		g->url->url->input().image(NULL);
	}

	// Is it a finished about:config? A more generic mechanism may be worth it later.
	if (val > 99 && cur->url && !strcmp(cur->url, "about:config")) {
		cur->web->bindEvent("input", "button", "click", configchange);
	}
}

static void statuscb(webview * const view) {
	tab * const cur = findtab(view);
	if (!cur || cur->state != TS_WEB)
		return;

	g->status->redraw();
}

static void historycb(const char *url, const char * /* title */, const time_t now) {
	g->history->add(url, now);
}

// Apply per-site settings when going to new sites
static void persite(webview * const view, const char * const url) {
	tab * const cur = findtab(view);
	if (!cur || cur->state != TS_WEB)
		return;

	char site[120];
	url2site(url, site, 120, true);

	setting *s = NULL;

	if (cur->js == TRI_AUTO) {
		s = getSetting("general.javascript", site);
		view->setBool(WK_SETTING_JS, s->val.u);
	}

	if (cur->css == TRI_AUTO) {
		s = getSetting("general.css", site);
		view->setBool(WK_SETTING_CSS, s->val.u);
	}

	if (cur->img == TRI_AUTO) {
		s = getSetting("general.images", site);
		view->setBool(WK_SETTING_IMG, s->val.u);
	}
}

static void errorcb(webview * const view, const char *err) {
	tab * const cur = findtab(view);
	if (!cur || cur->state != TS_WEB)
		return;

	cur->errors->add(err, 0);
	refresherr(cur->errors);
}

static void setcbs(webview * const web) {

	web->titleChangedCB(titlecb);
	web->loadStateChangedCB(stopcb);
	web->progressChangedCB(progresscb);
	web->faviconChangedCB(faviconcb);
	web->statusChangedCB(statuscb);
	web->historyAddCB(historycb);
	web->siteChangingCB(persite);
	web->errorCB(errorcb);
}

static void webdefaults(webview * const web) {

	const setting *s;

	s = getSetting("fonts.default");
	web->setChar(WK_SETTING_DEFAULT_FONT, s->val.c);

	s = getSetting("fonts.fixed");
	web->setChar(WK_SETTING_FIXED_FONT, s->val.c);

	s = getSetting("fonts.size");
	web->setInt(WK_SETTING_FONT_SIZE, s->val.u);

	s = getSetting("fonts.minsize");
	web->setInt(WK_SETTING_MINIMUM_FONT_SIZE, s->val.u);

	s = getSetting("fonts.fixedsize");
	web->setInt(WK_SETTING_FIXED_SIZE, s->val.u);
}

void newtab() {
	if (g->tabs.size()) {
		saveurlbar();
		if (g->tabs[g->curtab].web)
			g->tabs[g->curtab].web->hide();
	}

	Fl_Group::current(g->v);
	tab tab;
	tab.state = TS_SPEEDDIAL;
	tab.web = new webview(g->v->x(), g->v->y(), g->v->w(), g->v->h());
	g->tabs.push_back(tab);
	g->curtab = g->tabs.size() - 1;
	windowtitle();
	searchenginestate();
	urlbarstate();
	g->status->refreshzoom();
	g->w->redraw();
	g->url->url->take_focus();

	setcbs(tab.web);
	webdefaults(tab.web);
}

void closetab() {
	if (g->tabs.size() == 1) {
		if (g->tabs[0].state == TS_WEB) {
			saveurlbar();
			g->closedtabs.push_back(g->tabs[0]);
			g->tabs[0].web->hide();
		} else {
			if (g->tabs[0].web) {
				g->tabs[0].web->parent()->remove(g->tabs[0].web);
				delete g->tabs[0].web;
				delete g->tabs[0].errors;
			}
		}

		g->tabs.clear();
		newtab();
	} else {
		const vector<u16> &order = taborder();
		u16 next = order[1];
		if (next >= g->curtab)
			next--;

		if (g->tabs[g->curtab].state == TS_WEB) {
			saveurlbar();
			g->closedtabs.push_back(g->tabs[g->curtab]);
			g->tabs[g->curtab].web->hide();
		} else {
			if (g->tabs[g->curtab].web) {
				g->tabs[g->curtab].web->parent()->remove(g->tabs[g->curtab].web);
				delete g->tabs[g->curtab].web;
				delete g->tabs[g->curtab].errors;
			}
		}

		g->tabs.erase(g->tabs.begin() + g->curtab);
		g->curtab = next;
		activatetab(g->curtab);
	}
}

void newtab(const char *url) {
	if (g->tabs.size()) {
		saveurlbar();
		if (g->tabs[g->curtab].web)
			g->tabs[g->curtab].web->hide();
	}

	Fl_Group::current(g->v);
	tab tab;
	tab.web = new webview(g->v->x(), g->v->y(), g->v->w(), g->v->h());
	g->tabs.push_back(tab);
	g->curtab = g->tabs.size() - 1;
	urlbarstate();
	g->status->refreshzoom();
	g->w->redraw();

	setcbs(tab.web);
	webdefaults(tab.web);

	tab.web->load(url);
	tab.web->take_focus();
}

void newtabbg(const char *url) {
	if (g->tabs.size())
		saveurlbar();

	Fl_Group::current(g->v);
	tab tab;
	tab.web = new webview(g->v->x(), g->v->y(), g->v->w(), g->v->h());
	tab.lastactive = 0; // Put it in the back of the queue

	g->tabs.push_back(tab);
	g->tabwidget->redraw();

	setcbs(tab.web);
	webdefaults(tab.web);

	tab.web->load(url);
	tab.web->hide();
}

struct tmptab {
	u64 time;
	u16 i;
};

static int tabcmp(const void *ap, const void *bp) {
	const tmptab * const a = (const tmptab *) ap;
	const tmptab * const b = (const tmptab *) bp;

	if (a->time > b->time)
		return -1;
	else if (a->time < b->time)
		return 1;
	return 0;
}

vector<u16> taborder() {
	// Return the indices of all active tabs,
	// ordered by their access times in descending order.

	vector<u16> out;
	const u16 max = g->tabs.size();
	out.reserve(max);

	tmptab arr[max];

	u16 i, k = 0;
	for (i = 0; i < max; i++) {
		arr[k].time = g->tabs[i].lastactive;
		arr[k].i = i;
		k++;
	}

	qsort(arr, max, sizeof(struct tmptab), tabcmp);

	for (i = 0; i < max; i++) {
		out.push_back(arr[i].i);
	}

	return out;
}

void nexttab() {
	if (g->tabs.size() == 1)
		return;

	longpress++;
	longpress %= longorder.size();
	firstpress = false;
	g->w->redraw();
}

void prevtab() {
	if (g->tabs.size() == 1)
		return;

	if (longpress)
		longpress--;
	else
		longpress = longorder.size() - 1;
	firstpress = false;
	g->w->redraw();
}

void activatetab(const u16 tab) {

	if (g->tabs[g->curtab].web && g->curtab != tab) {
		g->tabs[g->curtab].web->hide();
		saveurlbar();
	}

	g->curtab = tab;
	g->tabs[g->curtab].lastactive = msec();

	if (g->tabs[g->curtab].web)
		g->tabs[g->curtab].web->show();

	windowtitle();
	searchenginestate();
	urlbarstate();
	g->status->hidefind();
	g->status->refreshzoom();

	g->w->redraw();

	if (g->tabs[g->curtab].state == TS_WEB && g->tabs[g->curtab].web)
		g->tabs[g->curtab].web->take_focus();
}

void startctrl() {
	longorder = taborder();
	longpress = 0;
	firstpress = true;
}

void endctrl() {
	if (!firstpress && longorder.size() > longpress)
		activatetab(longorder[longpress]);
	longorder.clear();
}

void undotab() {
	if (!g->closedtabs.size())
		return;

	const u32 which = g->closedtabs.size() - 1;

	g->tabs.push_back(g->closedtabs[which]);
	g->closedtabs.erase(g->closedtabs.begin() + which);
	activatetab(g->tabs.size() - 1);
}

tab *findtab(const webview * const view, bool closedok) {

	u32 max = g->tabs.size();
	u32 i;
	for (i = 0; i < max; i++) {
		if (view == g->tabs[i].web)
			return &g->tabs[i];
	}

	if (closedok) {
		max = g->closedtabs.size();
		for (i = 0; i < max; i++) {
			if (view == g->closedtabs[i].web)
				return &g->closedtabs[i];
		}
	}

	return NULL;
}
