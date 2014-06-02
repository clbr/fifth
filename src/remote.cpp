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

void *listenRemote(void *) {

	remotemsg m;

	long cur = fcntl(g->lockfd, F_GETFL);
	cur = cur & ~(O_NONBLOCK);
	if (fcntl(g->lockfd, F_SETFL, cur) != 0)
		die(_("Failed to block on lock file\n"));

	while (1) {
		m.data = NULL;
		ssize_t ret = read(g->lockfd, &m, 8);
		if (ret != 8) {
			if (errno)
				die(_("Failed reading remote\n"));
			else {
				sleep(1);
				continue;
			}
		}
		if (m.type >= RT_COUNT)
			err(_("Received unknown remote message, type %u\n"), m.type);

		if (m.size) {
			m.data = (char *) xcalloc(1, m.size + 1);
			if (read(g->lockfd, m.data, m.size) != m.size)
				die(_("Failed reading remote data\n"));
		}

		pthread_mutex_lock(&g->remotemutex);
		g->remotes.push_back(m);
		g->newremotes = 1;
		pthread_mutex_unlock(&g->remotemutex);
	}

	return NULL;
}

void sendRemote(const remotetype type, const u32 size, const char * const data) {

	if (type >= RT_COUNT)
		die(_("Attempted to send unknown type %u\n"), type);

	remotemsg m;
	m.type = type;
	m.size = size;

	if (write(g->lockfd, &m, 8) != 8)
		die(_("Failed to send remote call\n"));
	if (size && write(g->lockfd, data, size) != size)
		die(_("Failed to send remote call\n"));
}
