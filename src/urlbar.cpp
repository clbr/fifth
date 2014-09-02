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
#include "urlicons.h"
#include "searchicons.h"
#include "wmicon.h"
#include <FL/Fl_PNG_Image.H>
#include <algorithm>

Fl_PNG_Image *ddglogo, *googlelogo;

static void tabscb(Fl_Widget *w, void *) {
	vector<Fl_Menu_Item> items;
	const u32 max = g->closedtabs.size();
	items.reserve(max + 1);

	const int inactive = max ? 0 : FL_MENU_INACTIVE;

	Fl_Menu_Item empty = {_("Empty trash"), 0, 0, 0, FL_MENU_DIVIDER | inactive,
				FL_NORMAL_LABEL, FL_HELVETICA,
				FL_NORMAL_SIZE, FL_FOREGROUND_COLOR };
	items.push_back(empty);

	u32 i;
	for (i = 0; i < max; i++) {
		int shortcut = 0;
		if (!i) shortcut = menukey("keys.undotab");

		Fl_Menu_Item it = {strdup(g->closedtabs[i].title()),
					shortcut, 0, (void *) (unsigned long) (i + 1),
					0,
					FL_NORMAL_LABEL, FL_HELVETICA,
					FL_NORMAL_SIZE, FL_FOREGROUND_COLOR };
		items.push_back(it);
	}
	Fl_Menu_Item end;
	memset(&end, 0, sizeof(Fl_Menu_Item));
	items.push_back(end);

	const Fl_Menu_Item *ptr = items[0].popup(w->x(), w->y() + w->h());

	if (ptr) {
		i = (u64) ptr->user_data_;
		if (i) {
			// Restore it
			i--;
			g->tabs.push_back(g->closedtabs[i]);
			g->closedtabs.erase(g->closedtabs.begin() + i);
			activatetab(g->tabs.size() - 1);
		} else {
			// Clear all closed tabs
			for (i = 0; i < max; i++) {
				if (g->closedtabs[i].web) {
					g->closedtabs[i].web->parent()->remove(g->closedtabs[i].web);
					g->closedtabs[i].web->stop();

					delete g->closedtabs[i].errors;

					// Can't delete one with active downloads.
					if (g->closedtabs[i].web->numDownloads() == 0)
						delete g->closedtabs[i].web;
					else
						g->dlwebs.push_back(g->closedtabs[i].web);
				}
			}
			g->closedtabs.clear();
		}
	}

	for (i = 0; i < max; i++) {
		free((char *) items[i + 1].text);
	}
}

static void searchenginecb(Fl_Widget *w, void *) {

	const Fl_Menu_Button * const b = (Fl_Menu_Button *) w;
	const char * const label = b->text();
	g->url->search->input().placeholder(label);

	if (strcasestr(label, "duckduckgo")) {
		g->tabs[g->curtab].engine = TSE_DDG;
		g->url->search->input().image(ddglogo);
	} else if (strcasestr(label, "google")) {
		g->tabs[g->curtab].engine = TSE_GOOGLE;
		g->url->search->input().image(googlelogo);
	} else {
		die("Tried to set an unknown search engine\n");
	}

	g->url->search->redraw();
}

static void dosearch(Fl_Widget *w, void *) {
	const inputplace * const i = (inputplace *) w;
	const char * const val = i->value();
	if (strlen(val) < 2 || allspace(val))
		return;

	char *first = strdup(i->value());
	char *ptr = first;
	for (; *ptr; ptr++) {
		if (*ptr == ' ')
			*ptr = '+';
	}

	ptr = wk_urlencode(first);
	free(first);

	switch (g->tabs[g->curtab].engine) {
		case TSE_DDG:
			asprintf(&first, "https://duckduckgo.com/html?q=%s", ptr);
		break;
		case TSE_GOOGLE:
			asprintf(&first, "https://google.com/search?q=%s", ptr);
		break;
		case TSE_COUNT:
			die("Search engine corruption\n");
	}

	free(ptr);

	free((char *) g->tabs[g->curtab].search);
	g->tabs[g->curtab].search = strdup(i->value());

	if (i->shift && !i->ctrl) { // new tab
		newtab(first);
	} else if (i->shift && i->ctrl) { // bg tab
		newtabbg(first);
	} else {
		g->tabs[g->curtab].state = TS_WEB;
		g->tabs[g->curtab].web->load(first);
		g->tabs[g->curtab].web->take_focus();
	}

	free(first);
}

static void urlResults() {
	// Search through bookmarks and history, sort by how good a match each was
	const char * const needle = g->url->url->inp->value();

	struct res {
		const char *url;
		const char *name;
		u32 score;

		bool operator <(const res &other) const {
			return score > other.score; // Descending order
		}
	};

	vector<res> results;

	u32 i, max;
	max = g->history->size();
	for (i = 0; i < max; i++) {
		const int ret = ratedsearch(needle, g->history->getURL(i));
		if (ret < 1)
			continue;

		res r = {g->history->getURL(i), "", (u32) ret};
		results.push_back(r);
	}

	max = g->bookmarks.size();
	for (i = 0; i < max; i++) {
		const bookmark &cur = g->bookmarks[i];
		if (!cur.name || !cur.url)
			continue;

		int ret = ratedsearch(needle, cur.name);
		if (ret < 1)
			continue;
		int ret2 = ratedsearch(needle, cur.url);
		if (ret2 < 1)
			continue;

		res r = {cur.url, cur.name, (u32) ret + ret2};
		results.push_back(r);
	}

	g->url->url->list->clear();

	max = results.size();
	if (max < 1)
		return;

	std::sort(results.begin(), results.end());

	for (i = 0; i < max; i++) {
		char tmp[640];
		snprintf(tmp, 640, "%s\t%s", results[i].url, results[i].name);
		tmp[639] = '\0';
		g->url->url->list->add(tmp);
	}
}

static void dogo(Fl_Widget *w, void *) {
	inputplace * const i = (inputplace *) w;
	const char * const val = i->value();
	if (strlen(val) < 2 || allspace(val))
		return;

	// We need to differentiate between changed and enter-pressed states.
	if (i->changed()) {
		urlResults();
		if (g->url->url->list->size() > 2) {
			g->url->url->popup();
			i->take_focus();
		}
		return;
	}

	if (!strncmp(val, "javascript:", 11) && g->tabs[g->curtab].state == TS_WEB) {
		g->tabs[g->curtab].web->executeJS(val + 11);
		return;
	}

	if (i->shift && !i->ctrl) { // new tab
		newtab(val);
	} else if (i->shift && i->ctrl) { // bg tab
		newtabbg(val);
	} else {
		g->tabs[g->curtab].state = TS_WEB;
		g->tabs[g->curtab].web->load(val);
		g->tabs[g->curtab].web->take_focus();
	}
}

static void backcb(Fl_Widget *, void *) {
	back();
}

static void fwdcb(Fl_Widget *, void *) {
	fwd();
}

static void prevcb(Fl_Widget *, void *) {
	prev();
}

static void nextcb(Fl_Widget *, void *) {
	next();
}

static void stopcb(Fl_Widget *, void *) {
	if (g->url->isStop())
		stop();
	else
		refresh();
}

urlbar::urlbar(int x, int y, int w, int h): Fl_Group(x, y, w, h) {

	prev = new urlbutton(0, 0, 10, 10);
	back = new urlbutton(0, 0, 10, 10);
	fwd = new urlbutton(0, 0, 10, 10);
	next = new urlbutton(0, 0, 10, 10);
	refresh = new urlbutton(0, 0, 10, 10);

	url = new fl_browser_input(0, 0, 10, 10);
	begin(); // We need to still be current
	search = new textweb(0, 0, 10, 10);

	tabs = new urlbutton(0, 0, 10, 10);
	tabs->callback(tabscb);

	end();

	reposbuttons();

	// TODO: theming
	#define img(a) a, sizeof(a)
	googlelogo = new Fl_PNG_Image("google.png", img(google_png));
	ddglogo = new Fl_PNG_Image("ddg.png", img(ddg_png));

	refreshimg = new Fl_PNG_Image("refresh.png", img(reload_png));
	stopimg = new Fl_PNG_Image("stop.png", img(stop_png));

	// These are just loaded to cache for other widgets
	new Fl_PNG_Image("newtab.png", img(newtab_png));
	new Fl_PNG_Image("destop.png", img(destop_png));
	new Fl_PNG_Image("arrange.png", img(arrange_png));
	Fl_PNG_Image wmicon("wmicon.png", img(wmicon_png));
	g->w->icon(&wmicon);

	prev->image(new Fl_PNG_Image("prev.png", img(twoleftarrow_png)));
	back->image(new Fl_PNG_Image("back.png", img(leftarrow_png)));
	fwd->image(new Fl_PNG_Image("fwd.png", img(rightarrow_png)));
	next->image(new Fl_PNG_Image("next.png", img(tworightarrow_png)));

	prev->deimage(new Fl_PNG_Image("deprev.png", img(detwoleftarrow_png)));
	back->deimage(new Fl_PNG_Image("deback.png", img(deleftarrow_png)));
	fwd->deimage(new Fl_PNG_Image("defwd.png", img(derightarrow_png)));
	next->deimage(new Fl_PNG_Image("denext.png", img(detworightarrow_png)));
	refresh->deimage(new Fl_PNG_Image("dereload.png", img(dereload_png)));

	refreshstate(true);
	tabs->image(new Fl_PNG_Image("tabs.png", img(tabs_png)));
	#undef img

	url->input().placeholder(_("WWW address..."));
	url->input().callback(dogo);
	url->input().drawprogress = true;
	search->input().placeholder("DuckDuckGo");
	search->input().image(ddglogo);
	search->input().callback(dosearch);

	search->menubutton().add("DuckDuckGo", 0, 0);
	search->menubutton().add("Google", 0, 0);
	search->menubutton().callback(searchenginecb);

	prev->tooltip(_("First page in session"));
	back->tooltip(_("Back"));
	fwd->tooltip(_("Forward"));
	next->tooltip(_("Next page"));

	tabs->tooltip(_("Closed tabs"));

	prev->callback(prevcb);
	next->callback(nextcb);
	back->callback(backcb);
	fwd->callback(fwdcb);
	refresh->callback(stopcb);
}

void urlbar::draw() {

	if (damage() == FL_DAMAGE_CHILD) {
		draw_children();
		return;
	}

	const u32 startx = x();
	const u32 endx = x() + w() - 1;

	// Background
	u32 i;
	const u32 max = h() - 3;
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

	posy = y() + max + 2;
	fl_color(25, 35, 45);
	fl_line(startx, posy, endx, posy);

	// Kids
	draw_children();
}

void urlbar::resize(int x, int y, int w, int h) {

	Fl_Widget::resize(x, y, w, h);

	// Reposition buttons
	reposbuttons();
}

void urlbar::reposbuttons() {
	const u32 diff = 4;
	const u32 ydiff = 3;
	const u32 dim = h() - (ydiff * 2);

	prev->size(dim, dim);
	back->size(dim, dim);
	fwd->size(dim, dim);
	next->size(dim, dim);
	refresh->size(dim, dim);
	tabs->size(dim, dim);

	// TODO: removable buttons
	u32 pos = x() + diff;
	const u32 posy = y() + ydiff;

	prev->position(pos, posy);
	pos += diff + dim;

	back->position(pos, posy);
	pos += diff + dim;

	fwd->position(pos, posy);
	pos += diff + dim;

	next->position(pos, posy);
	pos += diff + dim;

	refresh->position(pos, posy);
	pos += diff + dim;

	const u32 urlstart = pos;

	// Tabs on the other edge
	pos = x() + w() - 1 - dim - diff;
	tabs->position(pos, posy);

	const u32 searchw = 170;
	pos -= diff + searchw;
	search->resize(pos, posy, searchw, dim);

	pos -= diff;

	url->resize(urlstart, posy, pos - urlstart, dim);
}

void urlbar::refreshstate(const bool green) {

	if (green) {
		refresh->image(refreshimg);
		refresh->tooltip(_("Refresh"));
	} else {
		refresh->image(stopimg);
		refresh->tooltip(_("Stop"));
	}
}

bool urlbar::isStop() const {
	return refresh->image() == stopimg;
}

void urlbuttonstate() {
	const tab * const cur = &g->tabs[g->curtab];
	if (cur->state != TS_WEB) {
		g->url->prev->deactivate();
		g->url->back->deactivate();
		g->url->fwd->deactivate();
		g->url->next->deactivate();
		g->url->refresh->deactivate();
	} else if (cur->web) {
		g->url->refresh->activate();
		g->url->next->activate();

		if (cur->web->isLoading())
			g->url->refreshstate(false);
		else
			g->url->refreshstate(true);

		if (cur->web->canBack()) {
			g->url->prev->activate();
			g->url->back->activate();
		} else {
			g->url->prev->deactivate();
			g->url->back->deactivate();
		}

		if (cur->web->canFwd()) {
			g->url->fwd->activate();
		} else {
			g->url->fwd->deactivate();
		}
	}
}
