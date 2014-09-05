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
#include <FL/fl_ask.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Tree_Item.H>

#define SSLCHECKFWD "ilovebieber"
#define SSLCHECKBACK "rebeibevoli"

#define DLBUTTONH 30
#define STOPW 80
#define REDOW 150

class sslview: public Fl_Group {
public:
	sslview(int x, int y, int w, int h): Fl_Group(x, y, w, h) {}
};

static void sslcb(Fl_Widget *w, void *url) {

	char site[120];
	url2site((const char *) url, site, 120, false);

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

static void opendl(const char path[]) {

	const setting *s = getSetting("exec.open");

	const pid_t pid = fork();
	if (pid == -1)
		die(_("Fork failed\n"));
	if (pid == 0) {
		execlp(s->val.c, s->val.c, path, NULL);
		exit(1);
	}
}

class delbrowser: public Fl_Hold_Browser {
public:
	delbrowser(int x, int y, int w, int h): Fl_Hold_Browser(x, y, w, h) {}

	int handle(const int e) override {
		if (e == FL_KEYDOWN) {
			const u32 key = Fl::event_key();
			if (key == FL_Delete) {
				if (value() > 1) {
					const dl * cur = (const dl *) data(value());
					cur->owner->removeDownload(cur->id);
					g->v->refreshdownloads(true);
				}
				return 1;
			} else if (key == FL_Enter) {
				if (value() > 1) {
					const dl * cur = (const dl *) data(value());
					opendl(cur->name);
				}
				return 1;
			}
		} else if (e == FL_PUSH && Fl::event_clicks()) {
			// Double click
			if (Fl::event_button() == FL_LEFT_MOUSE && value() > 1) {
				const dl * cur = (const dl *) data(value());
				opendl(cur->name);
			}
			return 1;
		}

		return Fl_Hold_Browser::handle(e);
	}
};

const dl *view::selecteddl() const {
	if (g->tabs[g->curtab].state != TS_DOWNLOAD)
		return NULL;
	const int val = dlbrowser->value();
	if (val < 2)
		return NULL;

	return (const dl *) dlbrowser->data(val);
}

static void dlstopcb(Fl_Widget *, void *) {

	const dl *cur = g->v->selecteddl();
	if (!cur) return;

	cur->owner->stopDownload(cur->id);
	g->v->refreshdownloads(true);
}

static void dlredocb(Fl_Widget *, void *) {

	const dl *cur = g->v->selecteddl();
	if (!cur) return;

	const char *url = strdup(cur->url);
	const char *name = strdup(cur->name);
	const char *last = strrchr(name, '/');

	cur->owner->stopDownload(cur->id);
	cur->owner->removeDownload(cur->id);

	cur->owner->download(url, last ? last + 1 : name);
	free((char *) url);
	free((char *) name);
}

static void dlcleancb(Fl_Widget *, void *) {

	while (1) {
		const vector<dl> &vec = getdownloads();
		const u32 max = vec.size();
		u32 i;

		if (!max)
			break;

		bool acted = false;
		for (i = 0; i < max; i++) {
			if (vec[i].finished) {
				vec[i].owner->removeDownload(vec[i].id);
				acted = true;
				// This may shift the other numbers, so re-calculate all.
				// Could be optimized, but not really important.
				break;
			}
		}

		if (!acted)
			break;
	}

	g->v->refreshdownloads(true);
}

static void dlbrowsercb(Fl_Widget *w, void *) {

	Fl_Hold_Browser *b = (Fl_Hold_Browser *) w;

	if (b->value() < 2) {
		b->select(1, 0);
	}

	g->v->redraw();
}

static void bookeditcb(Fl_Widget *, void *) {
	const Fl_Tree_Item * const item = g->v->selectedbookmark();
	if (!item)
		return;

	bookmark * const mark = (bookmark *) item->user_data();
	if (!mark)
		return;

	editbookmark(mark);
}

static void bookdelcb(Fl_Widget *, void *) {
	Fl_Tree_Item * const item = g->v->selectedbookmark();
	if (!item)
		return;

	bookmark * const mark = (bookmark *) item->user_data();
	if (!mark)
		return;

	g->v->removetree(item);
	g->v->noitem();
}

static void bookapplycb(Fl_Widget *, void *) {
	g->v->applytree();
}

static void bookdircb(Fl_Widget *, void *) {
	g->v->newdir();
}

static void bookmovecb(Fl_Widget *, void *) {
	// TODO
}

static void booktreecb(Fl_Widget *w, void *) {
	const Fl_Tree * const t = (Fl_Tree *) w;
	const Fl_Tree_Reason reason = t->callback_reason();
	switch (reason) {
		case FL_TREE_REASON_DRAGGED:
			g->v->treechanged();
		break;
		case FL_TREE_REASON_SELECTED:
			g->v->enableditem();
		break;
		case FL_TREE_REASON_DESELECTED:
			g->v->noitem();
		break;
		default:
		break;
	}
}

view::view(int x, int y, int w, int h): Fl_Group(x, y, w, h),
		mousex(0), mousey(0), mousein(false), downloads(UINT_MAX) {

	memset(dialicons, 0, 9 * sizeof(void *));
	memset(dialdeicons, 0, 9 * sizeof(void *));

	sslgroup = new sslview(x, y, w, h);

	ssltext = new Fl_Input(0, 0, 300, 25);
	sslbutton = new Fl_Button(0, 0, 200, 25, _("I understand, proceed"));
	sslbutton->hide();
	sslbutton->callback(sslcb);
	ssltext->hide();
	ssltext->user_data(sslbutton);

	sslgroup->end();

	dlgroup = new Fl_Group(x, y, w, h);

	dlstop = new Fl_Button(x + 3, y + 3, STOPW, DLBUTTONH);
	dlstop->image(Fl_Shared_Image::get("stop.png"));
	dlstop->deimage(Fl_Shared_Image::get("destop.png"));
	dlstop->label(_(" Stop"));
	dlstop->callback(dlstopcb);
	dlstop->align(FL_ALIGN_CENTER | FL_ALIGN_IMAGE_NEXT_TO_TEXT);

	dlredo = new Fl_Button(x + 3 + STOPW + 3, y + 3, REDOW, DLBUTTONH);
	dlredo->image(Fl_Shared_Image::get("refresh.png"));
	dlredo->deimage(Fl_Shared_Image::get("dereload.png"));
	dlredo->label(_(" Redownload"));
	dlredo->callback(dlredocb);
	dlredo->align(FL_ALIGN_CENTER | FL_ALIGN_IMAGE_NEXT_TO_TEXT);

	dlclean = new Fl_Button(dlredo->x() + REDOW + 3, y + 3, 120, DLBUTTONH);
	dlclean->image(Fl_Shared_Image::get("arrange.png"));
	dlclean->label(_(" Cleanup"));
	dlclean->tooltip(_("Remove all finished downloads from the list."));
	dlclean->callback(dlcleancb);
	dlclean->align(FL_ALIGN_CENTER | FL_ALIGN_IMAGE_NEXT_TO_TEXT);

	dlbrowser = new delbrowser(x, y + 3 + 3 + DLBUTTONH, w, h - 3 - 3 - DLBUTTONH);
	dlbrowser->column_char('\t');
	dlbrowser->callback(dlbrowsercb);

	dlgroup->end();

	bookgroup = new Fl_Group(x, y, w, h);

	bookedit = new Fl_Button(x + 3, y + 3, STOPW, DLBUTTONH);
	bookedit->label(_("Edit"));
	bookedit->callback(bookeditcb);

	bookdel = new Fl_Button(bookedit->x() + bookedit->w() + 3, y + 3, STOPW, DLBUTTONH);
	bookdel->label(_("Delete"));
	bookdel->callback(bookdelcb);

	bookdir = new Fl_Button(bookdel->x() + bookdel->w() + 3, y + 3, REDOW, DLBUTTONH);
	bookdir->label(_("New directory"));
	bookdir->callback(bookdircb);

	bookmove = new Fl_Button(bookdir->x() + bookdir->w() + 3, y + 3, REDOW, DLBUTTONH);
	bookmove->label(_("Move to directory"));
	bookmove->callback(bookmovecb);

	bookapply = new Fl_Button(bookmove->x() + bookmove->w() + 3, y + 3, REDOW, DLBUTTONH);
	bookapply->label(_("Apply changes"));
	bookapply->callback(bookapplycb);
	bookapply->deactivate();

	bookmarks = new Fl_Tree(x, y + 3 + 3 + DLBUTTONH, w, h - 3 - 3 - DLBUTTONH);
	bookmarks->showroot(0);
	bookmarks->selectmode(FL_TREE_SELECT_SINGLE_DRAGGABLE);
	bookmarks->callback(booktreecb);

	bookgroup->end();

	end();

	noitem();
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
		case TS_BOOKMARKS:
			drawbookmarks();
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
	dlredo->position(x + 3 + STOPW + 3, y + 3);
	dlclean->position(dlredo->x() + REDOW + 3, y + 3);
	dlbrowser->resize(x, y + 3 + 3 + DLBUTTONH, w, h - 3 - 3 - DLBUTTONH);

	int others = w / 8;
	int first = w / 2;
	if (others > 100) {
		others = 100;
		first = w - others * 4;
	}
	static int widths[6];
	widths[0] = first;
	widths[1] = others;
	widths[2] = others;
	widths[3] = others;
	widths[4] = others;
	widths[5] = 0;
	dlbrowser->column_widths(widths);

	// Bookmark ones
	bookedit->position(x + 3, y + 3);
	bookdel->position(bookedit->x() + bookedit->w() + 3, y + 3);
	bookdir->position(bookdel->x() + bookdel->w() + 3, y + 3);
	bookmove->position(bookdir->x() + bookdir->w() + 3, y + 3);
	bookapply->position(bookmove->x() + bookmove->w() + 3, y + 3);
	bookmarks->resize(x, y + 3 + 3 + DLBUTTONH, w, h - 3 - 3 - DLBUTTONH);

	// Invalidate speed dial icons on resize
	for (i = 0; i < 9; i++) {
		if (dialicons[i])
			delete dialicons[i];
		dialicons[i] = NULL;
		if (dialdeicons[i])
			delete dialdeicons[i];
		dialdeicons[i] = NULL;
	}
}

int view::handle(const int e) {

	if (g->tabs.size() < 1 || !g->run)
		return Fl_Widget::handle(e);

	tab * const cur = &g->tabs[g->curtab];

	if (e == FL_PUSH)
		g->status->hidefind();

	if (cur->state == TS_WEB && cur->web) {
		if (e == FL_MOUSEWHEEL && Fl::event_ctrl()) {
			g->status->wheelzoom();
			return 1;
		}

		if (e != FL_KEYDOWN && e != FL_KEYUP)
			return cur->web->handle(e);
	}

	switch (cur->state) {
		case TS_DOWNLOAD:
			if (e != FL_UNFOCUS)
				return dlgroup->handle(e);
		break;
		case TS_BOOKMARKS:
			if (e != FL_UNFOCUS)
				return bookgroup->handle(e);
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

							if (dialicons[tile])
								delete dialicons[tile];
							dialicons[tile] = NULL;
							if (dialdeicons[tile])
								delete dialdeicons[tile];
							dialdeicons[tile] = NULL;
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

static void maketrans(Fl_RGB_Image *img) {

	if (img->d() != 4 || !img->alloc_array)
		return;

	const u32 w = img->w();
	const u32 h = img->h();
	for (u32 y = 0; y < h; y++) {
		for (u32 x = 0; x < w; x++) {
			u8 *p = (u8 *) &img->array[y * w * 4 + x * 4];
			p[3] *= 0.5f;
		}
	}
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

		bool hover = false;
		if (mousein && mousex >= ex && mousex <= ex + ew &&
			mousey >= ey && mousey <= ey + eh) {
			fl_color(93, 102, 111);
			hover = true;
		} else {
			fl_color(63, 72, 81);
		}

		fl_rectf(ex + pad, ey + pad, ew - pad * 2, eh - pad * 2);

		fl_font(FL_HELVETICA, 12);
		fl_color(FL_WHITE);

		strcpy(tmp, "dial.1");
		tmp[5] += i;
		const setting * const s = getSetting(tmp, NULL);
		if (s->val.c && s->val.c[0]) {
			char site[64];
			url2site(s->val.c, site, 64);

			// Favicon
			Fl_RGB_Image *icon = dialicons[i];
			if (!icon) {
				icon = wk_get_favicon(s->val.c, 256);

				if (icon) {
					u32 iw = ew - pad * 2;
					u32 ih = eh - pad * 2;
					if (iw < ih)
						ih = iw;
					else
						iw = ih;

					if ((u32) icon->w() != iw) {
						Fl_RGB_Image *copy = (Fl_RGB_Image *)
								icon->copy(iw, ih);
						delete icon;
						icon = copy;
					}
					dialicons[i] = icon;
					dialdeicons[i] = (Fl_RGB_Image *) icon->copy();
					maketrans(dialdeicons[i]);
				}
			}

			if (icon) {
				if (!hover)
					icon = dialdeicons[i];
				icon->draw(ex + pad + ((ew - pad * 2) - icon->w()) / 2,
						ey + pad + ((eh - pad * 2) - icon->h()) / 2);
			}

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

	// If the amount of downloads changed, regenerate the widgets
	const vector<dl> &vec = getdownloads();
	const u32 total = vec.size();
	if (total != downloads) {
		lastdls = vec;
		regendl(lastdls);
		downloads = total;
	}

	if (downloads && dlbrowser->value() > 1) {
		dlstop->activate();
		dlredo->activate();
	} else {
		dlstop->deactivate();
		dlredo->deactivate();
	}

	draw_child(*dlstop);
	draw_child(*dlredo);
	draw_child(*dlclean);
	draw_child(*dlbrowser);
}

void view::regendl(const vector<dl> &vec) {

	const u32 max = vec.size();
	u32 i;

	// Keep selection if possible
	const int oldval = dlbrowser->value();
	const u32 oldsize = dlbrowser->size();

	dlbrowser->clear();
	char tmp[1024];
	const time_t now = time(NULL);

	dlbrowser->add(_("@B49Name\t@B49Size\t@B49Status\t@B49ETA\t@B49Speed"));

	for (i = 0; i < max; i++) {
		char size[80];
		char percent[80];
		char eta[80];
		char speed[80];
		time_t taken = now - vec[i].start;
		if (!taken) taken = 1;
		float avgspeed = (float) vec[i].received / taken;
		if (vec[i].size > 0) {
			float rounded = vec[i].size;
			const char *unit = "B";

			if (rounded > 1024) {
				unit = "KB";
				rounded /= 1024;
			}
			if (rounded > 1024) {
				unit = "MB";
				rounded /= 1024;
			}
			if (rounded > 1024) {
				unit = "GB";
				rounded /= 1024;
			}

			snprintf(size, 80, "%.1f %s", rounded, unit);
			snprintf(percent, 80, "%.1f%%", 100.0f * vec[i].received / vec[i].size);

			const long long remaining = vec[i].size - vec[i].received;
			float secs = remaining / avgspeed;
			unit = "s";

			if (secs > 120) {
				secs /= 60;
				unit = "min";
			}
			if (secs > 120) {
				secs /= 60;
				unit = "h";
			}

			snprintf(eta, 80, "%.0f %s", secs, unit);
		} else {
			strcpy(size, "?");
			strcpy(percent, "?");
			strcpy(eta, "?");
		}

		const char *unit = "B";

		if (avgspeed > 1024) {
			unit = "KB";
			avgspeed /= 1024;
		}
		if (avgspeed > 1024) {
			unit = "MB";
			avgspeed /= 1024;
		}
		if (avgspeed > 1024) {
			unit = "GB";
			avgspeed /= 1024;
		}

		snprintf(speed, 80, "%.1f %s/s", avgspeed, unit);

		if (vec[i].finished) {
			strcpy(percent, _("Finished"));
			strcpy(eta, _("Finished"));
		} else if (vec[i].failed) {
			strcpy(percent, _("Failed"));
			strcpy(eta, _("Never"));
		}

		// Name Size Percentage ETA Speed
		snprintf(tmp, 1024, "%s\t%s\t%s\t%s\t%s",
			vec[i].name, size, percent, eta, speed);
		dlbrowser->add(tmp, (void *) &vec[i]);
	}

	// Keep selection if possible
	if (oldval > 1 && oldsize == (u32) dlbrowser->size()) {
		dlbrowser->value(oldval);
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
				entry.id = d;
				entry.owner = g->tabs[i].web;
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
				entry.id = d;
				entry.owner = g->closedtabs[i].web;
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
				entry.id = d;
				entry.owner = g->dlwebs[i];
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

void view::refreshdownloads(const bool force) {
	static u64 last = 0;
	const u64 now = msec();
	if (force || now - last > 1000) {
		downloads = UINT_MAX;
		last = now;
		redraw();
	}
}

void view::drawbookmarks() {
	draw_child(*bookedit);
	draw_child(*bookdel);
	draw_child(*bookdir);
	draw_child(*bookmove);
	draw_child(*bookapply);
	draw_child(*bookmarks);
}

void view::regenbookmarks() {

	bookmarks->clear();
	bookapply->deactivate();

	const u32 max = g->bookmarks.size();
	u32 i;
	u32 depth = 0;
	string dirs[32];

	for (i = 0; i < max; i++) {
		const bookmark &cur = g->bookmarks[i];

		if (!cur.name) {
			depth--;
		} else if (!cur.url) {
			depth++;
			if (depth >= 32)
				die("Too deep bookmark hierarchy\n");
			dirs[depth] = cur.name;

			string tmp;
			u32 d;
			for (d = 1; d <= depth - 1; d++) {
				tmp += dirs[d];
				tmp += "/";
			}
			tmp += cur.name;

			Fl_Tree_Item * const it = bookmarks->add(tmp.c_str());
			it->user_data(&g->bookmarks[i]);
		} else {
			string tmp;
			u32 d;
			for (d = 1; d <= depth; d++) {
				tmp += dirs[d];
				tmp += "/";
			}
			tmp += cur.name;

			Fl_Tree_Item * const it = bookmarks->add(tmp.c_str());
			it->user_data(&g->bookmarks[i]);
		}
	}

	// Once created, assign folder icons
	Fl_Image *folder = Fl_Shared_Image::get("folder.png");
	for (Fl_Tree_Item *item = bookmarks->first(); item; item=item->next())
		if (item->has_children() || !item->user_data() ||
			!((bookmark *)item->user_data())->url) item->usericon(folder);
}

Fl_Tree_Item *view::selectedbookmark() const {
	return bookmarks->first_selected_item();
}

void view::removetree(Fl_Tree_Item *item) {
	bookmarks->remove(item);
	bookapply->activate();
	bookmarks->redraw();
}

void view::applytree() {

	vector<bookmark> news;
	Fl_Tree_Item *lastparent = NULL;

	for (Fl_Tree_Item *item = bookmarks->first(); item; item=item->next()) {
		if (item == bookmarks->root())
			continue;

		const bookmark * const mark = (const bookmark *) item->user_data();
		if (!mark || !mark->url) { // Dir
			bookmark tmp;
			tmp.name = strdup(item->label());
			tmp.url = NULL;
			news.push_back(tmp);

			lastparent = item;

			if (!item->next() || item->next()->parent() == item->parent()) {
				// Add an end-of-dir marker for empty dirs
				bookmark tmp;
				tmp.name = tmp.url = NULL;
				news.push_back(tmp);

				lastparent = item->parent();
			}
		} else {
			if (lastparent && lastparent != item->parent()) {
				bookmark tmp;
				tmp.name = tmp.url = NULL;
				news.push_back(tmp);

				lastparent = item->parent();
			}

			bookmark tmp;
			tmp.name = strdup(item->label());
			tmp.url = mark->url;
			news.push_back(tmp);
		}
	}

	g->bookmarks = news;

	savebookmarks();
	generatemenu();
	regenbookmarks();
	bookmarks->redraw();
}

void view::newdir() {
	Fl_Tree_Item * const it = bookmarks->add(_("New directory"));
	if (!it) {
		fl_alert(_("Could not create directory. Perhaps a same-named one exists?"));
		return;
	}

	Fl_Image *folder = Fl_Shared_Image::get("folder.png");
	it->usericon(folder);

	applytree();
}

void view::movetodir(Fl_Tree_Item *item, Fl_Tree_Item *dir) {

}

void view::treechanged() {
	bookapply->activate();
}

void view::noitem() {
	bookedit->deactivate();
	bookdel->deactivate();
	bookmove->deactivate();
}

void view::enableditem() {
	bookedit->activate();
	bookdel->activate();
	bookmove->activate();
}
