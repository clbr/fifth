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

#include "settings.h"

// Must be in alphabetical order.
const struct setting defaultSettings[] = {
	{"dial.1",		ST_CHAR, { .c = "" }},
	{"dial.2",		ST_CHAR, { .c = "" }},
	{"dial.3",		ST_CHAR, { .c = "" }},
	{"dial.4",		ST_CHAR, { .c = "" }},
	{"dial.5",		ST_CHAR, { .c = "" }},
	{"dial.6",		ST_CHAR, { .c = "" }},
	{"dial.7",		ST_CHAR, { .c = "" }},
	{"dial.8",		ST_CHAR, { .c = "http://www.phoronix.com/scan.php?page=home" }},
	{"dial.9",		ST_CHAR, { .c = "" }},
	{"exec.downloadnotify",	ST_CHAR, { .c = "flnotify -bg '#1e2832' -fg white -b -r -t 4" }},
	{"fltk.scheme",		ST_CHAR, { .c = "gtk+" }},
	{"general.downloaddir", ST_CHAR, { .c = "/tmp" }},
	{"general.homepage", 	ST_CHAR, { .c = "about:blank"}},
	{"general.javascript", 	ST_U32, { .u = 1}},
	{"general.startup", 	ST_U32, { .u = 0}},
	{"keys.addbookmark",	ST_U32, { .u = 33554532}}, // ctrl + d
	{"keys.back", 		ST_U32, { .u = 122}}, // z
	{"keys.closetab", 	ST_U32, { .u = 33554551}}, // ctrl + w
	{"keys.copy",		ST_U32, { .u = 33554531 }}, // ctrl + c
	{"keys.cut",		ST_U32, { .u = 33554552 }}, // ctrl + x
	{"keys.dial1",		ST_U32, { .u = 33554481 }}, // ctrl + 1
	{"keys.dial2",		ST_U32, { .u = 33554482 }}, // ctrl + 2
	{"keys.dial3",		ST_U32, { .u = 33554483 }}, // ctrl + 3
	{"keys.dial4",		ST_U32, { .u = 33554484 }}, // ctrl + 4
	{"keys.dial5",		ST_U32, { .u = 33554485 }}, // ctrl + 5
	{"keys.dial6",		ST_U32, { .u = 33554486 }}, // ctrl + 6
	{"keys.dial7",		ST_U32, { .u = 33554487 }}, // ctrl + 7
	{"keys.dial8",		ST_U32, { .u = 33554488 }}, // ctrl + 8
	{"keys.dial9",		ST_U32, { .u = 33554489 }}, // ctrl + 9
	{"keys.find",		ST_U32, { .u = 33554534 }}, // ctrl + f
	{"keys.find2",		ST_U32, { .u = 46 }}, // .
	{"keys.findnext",	ST_U32, { .u = 65472 }}, // F3
	{"keys.findprev",	ST_U32, { .u = 134283200 }}, // shift + F3
	{"keys.fwd", 		ST_U32, { .u = 120}}, // x
	{"keys.newtab",	 	ST_U32, { .u = 33554548}}, // ctrl + t
	{"keys.next",		ST_U32, { .u = 134217848}}, // shift + x
	{"keys.nexttab",	ST_U32, { .u = 33619721}}, // ctrl + tab
	{"keys.paste",		ST_U32, { .u = 33554550 }}, // ctrl + v
	{"keys.prev",		ST_U32, { .u = 134217850}}, // shift + z
	{"keys.prevtab",	ST_U32, { .u = 167837449}}, // ctrl + shift + tab
	{"keys.quit", 		ST_U32, { .u = 33554545}}, // ctrl + q
	{"keys.redo",		ST_U32, { .u = 33554546 }}, // ctrl + r
	{"keys.refresh",	ST_U32, { .u = 65474}}, // F5
	{"keys.screencap",	ST_U32, { .u = 33554547 }}, // ctrl + s
	{"keys.searchfield",	ST_U32, { .u = 134283205 }}, // shift + F8
	{"keys.selectall",	ST_U32, { .u = 33554529 }}, // ctrl + a
	{"keys.stop",		ST_U32, { .u = 65307 }}, // esc
	{"keys.transfers",	ST_U32, { .u = 100663412 }}, // ctrl + alt + t
	{"keys.undo",		ST_U32, { .u = 33554554 }}, // ctrl + z
	{"keys.undotab",	ST_U32, { .u = 100663418 }}, // ctrl + alt + z
	{"keys.urlfield",	ST_U32, { .u = 65477 }}, // F8
	{"window.bars", 	ST_CHAR, { .c = "tab,url,main"}},
	{"window.h", 		ST_U32, { .u = 600}},
	{"window.w", 		ST_U32, { .u = 800}},
	{"window.x", 		ST_U32, { .u = 0}},
	{"window.y", 		ST_U32, { .u = 0}},
};

const u32 numDefaults = sizeof(defaultSettings) / sizeof(struct setting);
