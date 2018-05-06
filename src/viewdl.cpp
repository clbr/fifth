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

const dl *view::selecteddl() const {
	if (g->tabs[g->curtab].state != TS_DOWNLOAD)
		return NULL;
	const int val = dlbrowser->value();
	if (val < 2)
		return NULL;

	return (const dl *) dlbrowser->data(val);
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
			strcpy(speed, _("Finished"));
		} else if (vec[i].failed) {
			strcpy(percent, _("Failed"));
			strcpy(eta, _("Never"));
			strcpy(speed, _("Failed"));
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
		if ((g->tabs[i].state == TS_WEB || g->tabs[i].state == TS_SSLERR) &&
			g->tabs[i].web &&
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
		if ((g->closedtabs[i].state == TS_WEB || g->closedtabs[i].state == TS_SSLERR) &&
			g->closedtabs[i].web &&
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
