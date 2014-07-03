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

void loadkeys() {

}

u32 keytou32(const u32 key, const bool ctrl, const bool alt, const bool shift) {
	u32 out = key;
	if (ctrl)
		out |= KEYBIT_CTRL;
	if (alt)
		out |= KEYBIT_ALT;
	if (shift)
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
