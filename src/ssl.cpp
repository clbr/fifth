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

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

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

static u8 isletsencrypt(const char * const in) {

	#define AKLEN 20

	static const char tgtname[] = "/C=US/O=Let's Encrypt/CN=Let's Encrypt Authority X3";
	static const u8 tgtak[AKLEN] = {
		0xa8,0x4a,0x6a,0x63,0x04,0x7d,0xdd,0xba,
		0xe6,0xd1,0x39,0xb7,0xa6,0x45,0x65,0xef,
		0xf3,0xa8,0xec,0xa1
	};
	char buf[1024];
	u16 len;
	const u8 *ak;

	BIO *bio = BIO_new_mem_buf((u8 *) in, strlen(in));
	X509 *info = PEM_read_bio_X509(bio, NULL, NULL, NULL);
	BIO_free_all(bio);

	if (!info)
		goto notlets;

	X509_NAME_oneline(info->cert_info->issuer, buf, 1024);
	if (strcmp(tgtname, buf))
		goto notlets;

	info->akid = (AUTHORITY_KEYID *)
			X509_get_ext_d2i(info, NID_authority_key_identifier, NULL, NULL);
	len = ASN1_STRING_length(info->akid->keyid);
	ak = ASN1_STRING_data(info->akid->keyid);

	if (len != AKLEN)
		goto notlets;

	if (memcmp(ak, tgtak, AKLEN))
		goto notlets;

	X509_free(info);
	return 1;

	notlets:

	X509_free(info);
	return 0;

	#undef AKLEN

	/*
	Yes, an attacker could forge a cert pretending to be Let's Encrypt.
	However, that case is indistinguishable from a real cert from them
	that updates every N days.
	*/
}

int certcheck(const char *str, const char *host) {

	char site[128];
	url2site(host, site, 128, false);

	if (skipcert(site))
		return 1;

	// Also skip let's encrypt.
	if (isletsencrypt(str))
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
