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

view::view(int x, int y, int w, int h): Fl_Group(x, y, w, h),
		mousex(0), mousey(0), mousein(false) {
	end();
}

void view::draw() {

	tab * const cur = &g->tabs[g->curtab];

	// TODO drawing
	switch (cur->state) {
		case TS_WEB:
			if (cur->web)
				cur->web->draw();
		break;
		case TS_DOWNLOAD:
		break;
		case TS_SSLERR:
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
}

int view::handle(const int e) {

	if (g->tabs.size() < 1 || !g->run)
		return Fl_Widget::handle(e);

	tab * const cur = &g->tabs[g->curtab];

	if (cur->state == TS_WEB && cur->web) {
		if (e != FL_KEYDOWN && e != FL_KEYUP)
			return cur->web->handle(e);
	}

	switch (cur->state) {
		case TS_DOWNLOAD:
		break;
		case TS_SSLERR:
		break;
		case TS_SPEEDDIAL:
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
			if (!memcmp(site, "www.", 4)) {
				const u32 len = strlen(site);
				memmove(site, site + 4, len - 4);
				site[len - 4] = '\0';
			}
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
