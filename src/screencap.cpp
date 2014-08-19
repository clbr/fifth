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
#include <FL/Fl_File_Chooser.H>

void screencap() {
	const tab * const cur = &g->tabs[g->curtab];
	if (cur->state != TS_WEB) {
		fl_alert(_("The current tab is not a web view."));
		return;
	}

	setting *s = getSetting("general.downloaddir", NULL);
	Fl_File_Chooser c(s->val.c, _("PNG files (*.png)"),
				Fl_File_Chooser::CREATE, _("Save screencap as"));
	c.show();

	while (c.shown())
		Fl::wait();
	if (!c.value())
		return;

	cur->web->snapshot(c.value());
}
