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

static keybinding settingkey(const char * const name) {
	setting *s = getSetting(name, NULL);
	return u32tokey(s->val.u);
}

static void back() {
	puts("back");
}

static void fwd() {
	puts("fwd");
}

static void quit() {
	g->w->hide();
}

static void urlfocus() {
	g->url->url->input().position(0, g->url->url->input().size());
	g->url->url->input().take_focus();
}

static void searchfocus() {
	g->url->search->input().position(0, g->url->search->input().size());
	g->url->search->input().take_focus();
}

void loadkeys() {
	g->keys.clear();

	keybinding key;

	key = settingkey("keys.back");
	g->keys[key] = back;

	key = settingkey("keys.fwd");
	g->keys[key] = fwd;

	key = settingkey("keys.newtab");
	g->keys[key] = newtab;

	key = settingkey("keys.closetab");
	g->keys[key] = closetab;

	key = settingkey("keys.quit");
	g->keys[key] = quit;

	key = settingkey("keys.screencap");
	g->keys[key] = screencap;

	key = settingkey("keys.nexttab");
	g->keys[key] = nexttab;

	key = settingkey("keys.prevtab");
	g->keys[key] = prevtab;

	key = settingkey("keys.undotab");
	g->keys[key] = undotab;

	key = settingkey("keys.urlfield");
	g->keys[key] = urlfocus;

	key = settingkey("keys.searchfield");
	g->keys[key] = searchfocus;
}

u32 keytou32(const keybinding &key) {
	u32 out = key.key;
	if (key.ctrl)
		out |= KEYBIT_CTRL;
	if (key.alt)
		out |= KEYBIT_ALT;
	if (key.shift)
		out |= KEYBIT_SHIFT;

	return out;
}

keybinding u32tokey(const u32 in) {

	keybinding k;

	k.ctrl = k.alt = k.shift = false;

	if (in & KEYBIT_CTRL)
		k.ctrl = true;
	if (in & KEYBIT_ALT)
		k.alt = true;
	if (in & KEYBIT_SHIFT)
		k.shift = true;

	k.key = in & 0xfffff;

	return k;
}

u32 menukey(const char * const name) {

	keybinding k = settingkey(name);
	u32 out = k.key;

	if (k.ctrl)
		out += FL_CTRL;
	if (k.alt)
		out += FL_ALT;
	if (k.shift)
		out += FL_SHIFT;

	return out;
}
