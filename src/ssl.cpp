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

static u8 domaintailmatch(const char * const str, const char * const end) {

	const u32 len = strlen(str);
	const u32 endlen = strlen(end);

	if (endlen > len)
		return 0;

	if (len > endlen &&
		str[len - endlen - 1] != '.' &&
		str[len - endlen - 1] != '/')
		return 0;

	return strcmp(&str[len - endlen], end) == 0;
}

/* Google actively works against security by changing their certs
   several times a week. Since having the user click through is
   worthless, whitelist google sites. */
static const char * const whited[] = {
	"google.fi",
	"google.com",
	"googlecode.com",
	"googledrive.com",
	"googlegroups.com",
	"googleusercontent.com",
	"googlevideo.com",
	"googleapis.com",
	"goo.gl",
	"gstatic.com",
	"youtube.com",
	"ytimg.com",
};

static u8 skipcert(const char *host) {

	u32 i;
	for (i = 0; i < sizeof(whited) / sizeof(whited[0]); i++) {
		if (domaintailmatch(host, whited[i]))
			return 1;
	}

	return 0;
}

int certcheck(const char *str, const char *host) {

	char site[128];
	url2site(host, site, 128, false);

	if (skipcert(site))
		return 1;

	const u32 len = strlen(str);
	int fd = openat(g->certfd, site, O_RDONLY);
	if (fd >= 0) {
		// It must match byte-to-byte with the new one.
		char buf[len];
		if (sread(fd, buf, len) != len) {
			close(fd);
			return 0;
		}
		close(fd);

		if (memcmp(buf, str, len)) {
			return 0;
		}
	} else {
		// Never seen this site before, save its cert
		// TODO option for asking the user on all new certs
		fd = openat(g->certfd, site, O_WRONLY | O_CREAT, 0644);
		if (fd < 0)
			die(_("Failed to create the certificate file \"%s\", %s\n"), site, strerror(errno));

		if (swrite(fd, str, len) != len)
			die(_("Failed writing the certificate\n"));
		close(fd);
	}

	return 1;
}

void certerr(webview *view, const char *url, const bool sub) {

	// Find the tab with this view
	tab * const cur = findtab(view);
	if (!cur)
		return;

	// Stop loading it was a sub-resource. If it was main, stop makes no sense, and
	// crashes as well.
	if (sub)
		cur->web->stop();

	cur->state = TS_SSLERR;
	free((char *) cur->sslsite);
	cur->sslsite = strdup(url);
	urlbuttonstate();
	g->v->resetssl();
	g->w->redraw();
}
