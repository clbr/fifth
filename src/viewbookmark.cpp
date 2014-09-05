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

			if (lastparent && !item->next() &&
				item->parent() != bookmarks->root()) {
				bookmark tmp;
				tmp.name = tmp.url = NULL;
				news.push_back(tmp);

				lastparent = item->parent();
			}
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

void view::listdirs(Fl_Tree *out) const {

	out->clear();
	out->showroot(1);
	Fl_Image *folder = Fl_Shared_Image::get("folder.png");

	for (Fl_Tree_Item *item = bookmarks->first(); item; item=item->next()) {
		if (item == bookmarks->root())
			continue;

		const bookmark * const mark = (const bookmark *) item->user_data();
		if (!mark || !mark->url) { // Dir
			Fl_Tree_Item *added = out->add(item->label());
			added->user_data(item);
			added->usericon(folder);
		}
	}

	out->root()->usericon(folder);
	out->root()->user_data(bookmarks->root());
}
