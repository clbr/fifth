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

int certcheck(const char *str, const char *host) {

	char site[128];
	url2site(host, site, 128);

	const u32 len = strlen(str);
	int fd = openat(g->certfd, site, O_RDONLY);
	if (fd >= 0) {
		// It must match byte-to-byte with the new one.
		char buf[len];
		if (read(fd, buf, len) != len)
			die(_("Failed reading the certificate\n"));
		close(fd);

		if (memcmp(buf, str, len)) {
			// TODO big scary SSL warning
			return 0;
		}
	} else {
		// Never seen this site before, save its cert
		// TODO option for asking the user on all new certs
		fd = openat(g->certfd, site, O_WRONLY | O_CREAT);
		if (fd < 0)
			die(_("Failed to create the certificate file\n"));

		if (write(fd, str, len) != len)
			die(_("Failed writing the certificate\n"));
		close(fd);
	}

	return 1;
}
