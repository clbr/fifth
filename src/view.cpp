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
#include "FL/fl_ask.H"

#define SSLCHECKFWD "ilovebieber"
#define SSLCHECKBACK "rebeibevoli"

class sslview: public Fl_Group {
public:
	sslview(int x, int y, int w, int h): Fl_Group(x, y, w, h) {}
};

static void sslcb(Fl_Widget *w, void *url) {

	char site[120];
	url2site((const char *) url, site, 120);

	// Was it typed correctly?
	Fl_Group *parent = w->parent();
	const u32 num = parent->children();
	u32 i;
	Fl_Input *text = NULL;
	for (i = 0; i < num; i++) {
		Fl_Widget *t = parent->child(i);
		if (t == w)
			continue;
		if (t->user_data() == w) {
			text = (Fl_Input *) t;
			break;
		}
	}

	if (!text)
		return;

	if (strcmp(SSLCHECKFWD, text->value())) {
		fl_alert(_("Text doesn't match."));
	} else {
		fl_alert(_("Erasing the old certificate and returning to web view."));
		if (unlinkat(g->certfd, site, 0))
			die(_("Error removing certificate\n"));
		g->tabs[g->curtab].state = TS_WEB;
		g->w->redraw();
		urlbuttonstate();
	}
}

static void dlstopcb(Fl_Widget *, void *) {

}

static void dlredocb(Fl_Widget *, void *) {

}

view::view(int x, int y, int w, int h): Fl_Group(x, y, w, h),
		mousex(0), mousey(0), mousein(false), downloads(UINT_MAX) {

	sslgroup = new sslview(x, y, w, h);

	ssltext = new Fl_Input(0, 0, 300, 25);
	sslbutton = new Fl_Button(0, 0, 200, 25, _("I understand, proceed"));
	sslbutton->hide();
	sslbutton->callback(sslcb);
	ssltext->hide();
	ssltext->user_data(sslbutton);

	sslgroup->end();

	dlgroup = new Fl_Group(x, y, w, h);

	dlstop = new Fl_Button(x + 3, y + 3, 100, 40);
	dlstop->image(Fl_Shared_Image::get("stop.png"));
	dlstop->deimage(Fl_Shared_Image::get("destop.png"));
	dlstop->label(_(" Stop"));
	dlstop->callback(dlstopcb);
	dlstop->align(FL_ALIGN_CENTER | FL_ALIGN_IMAGE_NEXT_TO_TEXT);
	dlstop->show();

	dlredo = new Fl_Button(x + 3 + 100 + 3, y + 3, 150, 40);
	dlredo->image(Fl_Shared_Image::get("refresh.png"));
	dlredo->deimage(Fl_Shared_Image::get("dereload.png"));
	dlredo->label(_(" Redownload"));
	dlredo->callback(dlredocb);
	dlredo->align(FL_ALIGN_CENTER | FL_ALIGN_IMAGE_NEXT_TO_TEXT);
	dlredo->show();

	dlgroup->end();

	end();
}

void view::draw() {

	tab * const cur = &g->tabs[g->curtab];

	if (cur->state != TS_SSLERR) {
		sslbutton->hide();
		ssltext->hide();
	}

	switch (cur->state) {
		case TS_WEB:
			if (cur->web)
				cur->web->draw();
		break;
		case TS_DOWNLOAD:
			drawdl();
		break;
		case TS_SSLERR:
			ssltext->show();
			sslbutton->show();
			sslbutton->user_data((void *) cur->sslsite);

			drawssl();
			draw_child(*ssltext);
			draw_child(*sslbutton);
		break;
		case TS_SPEEDDIAL:
			drawdial();
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

	// Download button positions
	dlstop->position(x + 3, y + 3);
	dlredo->position(x + 3 + 100 + 3, y + 3);
}

int view::handle(const int e) {

	if (g->tabs.size() < 1 || !g->run)
		return Fl_Widget::handle(e);

	tab * const cur = &g->tabs[g->curtab];

	if (e == FL_PUSH)
		g->status->hidefind();

	if (cur->state == TS_WEB && cur->web) {
		if (e != FL_KEYDOWN && e != FL_KEYUP)
			return cur->web->handle(e);
	}

	switch (cur->state) {
		case TS_DOWNLOAD:
			if (e != FL_UNFOCUS)
				return dlgroup->handle(e);
		break;
		case TS_SSLERR:
			if (e != FL_UNFOCUS)
				return sslgroup->handle(e);
		break;
		case TS_SPEEDDIAL:
		{
			u32 ew, eh, pad, totalw, totalh, startx, starty;

			if (e != FL_MOVE)
				dialdims(&startx, &starty, &totalw, &totalh, &pad, &ew, &eh);

			switch (e) {
				case FL_ENTER:
					mousein = true;
					return 1;
				case FL_LEAVE:
					mousein = false;
					return 1;
				case FL_MOVE:
					mousex = Fl::event_x();
					mousey = Fl::event_y();
					redraw();
				break;
				case FL_PUSH:
					mousex = Fl::event_x();
					mousey = Fl::event_y();

					// Inside the dial area?
					if (mousex < startx || mousex >= startx + totalw ||
						mousey < starty || mousey >= starty + totalh)
						break;
					// Inside a tile?
					u32 i;
					u32 tile = INT_MAX;
					u32 ex, ey;
					for (i = 0; i < 9; i++) {
						const u32 col = i % 3;
						const u32 row = i / 3;

						ex = startx + col * (ew + pad);
						ey = starty + row * (eh + pad);
						if (mousex >= ex && mousex <= ex + ew &&
							mousey >= ey && mousey <= ey + eh) {
							tile = i;
							break;
						}
					}

					if (tile >= 9)
						break;

					char tmp[10] = "dial.1";
					tmp[5] += tile;
					setting * const s = getSetting(tmp, NULL);
					if (s->val.c && s->val.c[0]) {
						// Delete cross or normal click?
						const u32 cornerx1 = ex + ew - pad;
						const u32 cornerx2 = ex + ew;
						const u32 cornery1 = ey;
						const u32 cornery2 = ey + pad;
						if (mousein && mousex >= cornerx1 &&
							mousex <= cornerx2 &&
							mousey >= cornery1 &&
							mousey <= cornery2) {
							// Yes, known leak.
							s->val.c = "";
						} else {
							g->tabs[g->curtab].state = TS_WEB;
							g->tabs[g->curtab].web->load(s->val.c);
							g->tabs[g->curtab].web->take_focus();
						}
					} else {
						// Add new one
						adddial(tile);
					}
				break;
			}
		}
		break;
		case TS_WEB:
		break;
		case TS_COUNT:
			die("Not reached\n");
	}

	return Fl_Widget::handle(e);
}

void view::dialdims(u32 *dx, u32 *dy, u32 *dw, u32 *dh, u32 *pad,
			u32 *ew, u32 *eh) const {
	*pad = 20;

	// Calculate the dimensions of one box.
	if (w() > h()) {
		u32 amount = h() - 2 * *pad;
		amount /= 4;
		*eh = amount;
		*ew = amount * 1.333f;
	} else {
		u32 amount = w() - 2 * *pad;
		amount /= 4;
		*ew = amount;
		*eh = amount / 1.333f;
	}

	const u32 totalw = *ew * 3 + *pad * 2;
	const u32 totalh = *eh * 3 + *pad * 2;
	const u32 startx = x() + (w() - totalw) / 2;
	const u32 starty = y() + (h() - totalh) / 2;

	*dx = startx;
	*dy = starty;
	*dw = totalw;
	*dh = totalh;
}

void view::drawdial() {

	u32 ew, eh, pad, totalw, totalh, startx, starty;
	dialdims(&startx, &starty, &totalw, &totalh, &pad, &ew, &eh);

	u32 i;
	for (i = 0; i < 9; i++) {
		const u32 col = i % 3;
		const u32 row = i / 3;

		const u32 ex = startx + col * (ew + pad);
		const u32 ey = starty + row * (eh + pad);

		fl_color(80, 90, 100);
		fl_rectf(ex, ey, ew, eh);

		fl_color(FL_WHITE);
		fl_font(FL_HELVETICA + FL_BOLD, 14);

		char tmp[10] = "1";
		tmp[0] += i;

		fl_draw(tmp, ex, ey + 3, pad, pad, FL_ALIGN_CENTER);

		fl_font(FL_HELVETICA, 12);

		strcpy(tmp, "dial.1");
		tmp[5] += i;
		const setting * const s = getSetting(tmp, NULL);
		if (s->val.c && s->val.c[0]) {
			char site[64];
			url2site(s->val.c, site, 64);

			// TODO favicon
			fl_draw(site, ex + pad, ey + eh - pad, ew - 2 * pad, pad,
				FL_ALIGN_CENTER);

			// Deletion cross
			const u32 cornerx1 = ex + ew - pad;
			const u32 cornerx2 = ex + ew;
			const u32 cornery1 = ey;
			const u32 cornery2 = ey + pad;
			if (mousein && mousex >= cornerx1 && mousex <= cornerx2 &&
				mousey >= cornery1 && mousey <= cornery2) {
				fl_color(FL_WHITE);
			} else {
				fl_color(113, 122, 131);
			}

			const u32 off = pad / 3;
			fl_line(cornerx1 + off, cornery1 + off,
				cornerx2 - off, cornery2 - off);
			fl_line(cornerx1 + off, cornery2 - off,
				cornerx2 - off, cornery1 + off);
		} else {
			if (mousein && mousex >= ex && mousex <= ex + ew &&
				mousey >= ey && mousey <= ey + eh) {
				fl_color(93, 102, 111);
			} else {
				fl_color(63, 72, 81);
			}

			fl_rectf(ex + pad, ey + pad, ew - pad * 2, eh - pad * 2);

			fl_color(FL_WHITE);
			fl_draw(_("Add..."), ex + pad, ey + pad, ew - pad * 2, eh - pad * 2,
				FL_ALIGN_CENTER);
		}
	}
}

void view::drawssl() {

	tab * const cur = &g->tabs[g->curtab];
	const u32 bigsize = 32;
	const u32 smallsize = bigsize / 2;

	fl_color(FL_BLACK);
	fl_rectf(x(), y(), w(), h());

	fl_color(FL_WHITE);
	fl_font(FL_HELVETICA, bigsize);

	u32 cury = y() + bigsize * 2;

	fl_draw(_("Security risk: certificate changed!"), x(), cury, w(), bigsize,
		FL_ALIGN_CENTER);

	cury += bigsize * 2;
	fl_font(FL_HELVETICA, smallsize);

	char tmp[320];
	snprintf(tmp, 320, _("Site: %s"), cur->sslsite);
	tmp[319] = '\0';

	fl_draw(tmp, x(), cury, w(), smallsize, FL_ALIGN_CENTER);

	cury += fl_height() * 3;
	fl_draw(_("This might be a man-in-the-middle attack. You're advised to close this tab."),
		x(), cury, w(), smallsize, FL_ALIGN_CENTER);

	snprintf(tmp, 320, _("If you're absolutely certain you want to continue at your risk, "
				"to make sure you understand the gravity of the situation, "
				"type %s backwards into the field below."),
			SSLCHECKBACK);
	cury += fl_height() * 3;
	fl_draw(tmp, x() + 200, cury, w() - 400, smallsize * 5, FL_ALIGN_CENTER | FL_ALIGN_WRAP);

	cury += fl_height() * 5;
	ssltext->resize(x() + 200, cury, ssltext->w(), ssltext->h());
	((Fl_Widget *) ssltext)->draw();

	sslbutton->position(x() + 200 + 6 + ssltext->w(), cury);
	((Fl_Widget *) sslbutton)->draw();
}

void view::resetssl() {
	ssltext->value("");
}

void view::drawdl() {

	draw_child(*dlstop);
	draw_child(*dlredo);

	// If the amount of downloads changed, regenerate the widgets
	const vector<dl> &vec = getdownloads();
	const u32 total = vec.size();
	if (total != downloads) {
		regendl(vec);
		downloads = total;
	}
}

void view::regendl(const vector<dl> &vec) {

	const u32 max = vec.size();
	u32 i;

	for (i = 0; i < max; i++) {
		printf("Download %u: %s %s %lld/%lld finished %u failed %u %lu\n",
			i, vec[i].name, vec[i].url, vec[i].received, vec[i].size,
			vec[i].finished, vec[i].failed, vec[i].start);
	}
}

vector<dl> getdownloads() {
	u32 i, max, d, dls;
	vector<dl> vec;
	vec.reserve(256);

	max = g->tabs.size();
	for (i = 0; i < max; i++) {
		if (g->tabs[i].state == TS_WEB && g->tabs[i].web &&
			g->tabs[i].web->numDownloads()) {
			dls = g->tabs[i].web->numDownloads();
			for (d = 0; d < dls; d++) {
				dl entry;
				g->tabs[i].web->downloadStats(d, &entry.start,
						&entry.size, &entry.received,
						&entry.name, &entry.url);
				entry.finished = g->tabs[i].web->downloadFinished(d);
				entry.failed = g->tabs[i].web->downloadFailed(d);
				vec.push_back(entry);
			}
		}
	}

	max = g->closedtabs.size();
	for (i = 0; i < max; i++) {
		if (g->closedtabs[i].state == TS_WEB && g->closedtabs[i].web &&
			g->closedtabs[i].web->numDownloads()) {
			dls = g->closedtabs[i].web->numDownloads();
			for (d = 0; d < dls; d++) {
				dl entry;
				g->closedtabs[i].web->downloadStats(d, &entry.start,
						&entry.size, &entry.received,
						&entry.name, &entry.url);
				entry.finished = g->closedtabs[i].web->downloadFinished(d);
				entry.failed = g->closedtabs[i].web->downloadFailed(d);
				vec.push_back(entry);
			}
		}
	}

	max = g->dlwebs.size();
	for (i = 0; i < max; i++) {
		if (g->dlwebs[i]->numDownloads()) {
			dls = g->dlwebs[i]->numDownloads();
			for (d = 0; d < dls; d++) {
				dl entry;
				g->dlwebs[i]->downloadStats(d, &entry.start,
						&entry.size, &entry.received,
						&entry.name, &entry.url);
				entry.finished = g->dlwebs[i]->downloadFinished(d);
				entry.failed = g->dlwebs[i]->downloadFailed(d);
				vec.push_back(entry);
			}
		}
	}

	// Erasing check
	for (i = 0; i < g->dlwebs.size(); i++) {
		if (!g->dlwebs[i]->numDownloads()) {
			g->dlwebs.erase(g->dlwebs.begin() + i);
			i--;
		}
	}

	return vec;
}
