#include <net/net.h>
#include <net/url.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void
url_build(char __inout *dst, uint32_t len, url_t url) {
	uint32_t tmplen;
	char buf[256] = {0};

	if(dst == NULL || len == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	memset(dst, 0, len);
	--len; // '\0'

	tmplen = strlen(url.scheme);
	if(tmplen < len && len != 0) {
		snprintf(dst, len, "%s://", url.scheme);
		
		len -= (tmplen + 3);
	}

	if(strlen(url.auth.user) != 0 && strlen(url.auth.pass) != 0) {
		tmplen = strlen(url.auth.user) + strlen(url.auth.pass) + 2;
		snprintf(dst, len, "%s%s:%s@", dst, url.auth.user, url.auth.pass);
	} else if(strlen(url.auth.user) != 0) {
		tmplen = strlen(url.auth.user) + 1;
		snprintf(dst, len, "%s%s@", dst, url.auth.user);
	}

	len -= tmplen;

	snprintf(dst, len, "%s%s", dst, url.host);
	len -= strlen(url.host);

	if(url.port != 0) {
		snprintf(buf, 255, "%d", url.port);
		tmplen = strlen(buf) + 1;

		if(tmplen > 0 && len > tmplen) {
			snprintf(dst, len, "%s:%s", dst, buf);
		}
	}

	tmplen = strlen(url.path);

	if(tmplen < len) {
		snprintf(dst, len, "%s%s", dst, url.path);
		len -= tmplen;
	}

	tmplen = strlen(url.vars) + 1;
	if(tmplen < len) {
		snprintf(dst, len, "%s?%s", dst, url.vars);
		len -= tmplen;
	}
}

void
url_parse(char __in *src, url_t __inout *url) {
	char *tmp, *last = src, *tok;

	if(src == 0 || url == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	memset(url, 0, sizeof(url_t));

	tmp = strstr(src, "://");
	if(tmp != NULL) {
		if((tmp - src) < LIBNET_URL_SIZE_SCHEME) {
			strncpy(url->scheme, src, (tmp - src));
		}

		last = tmp + 3;
	}

	tmp = strstr(tmp, "@");
	if(tmp != NULL) {
		if(strstr(src, ":") == NULL) {
			strncpy(url->auth.user, last, (tmp - last));
			last = tmp;
		} else {
			tok = strtok(last, ":");
			strncpy(url->auth.user, tok, strlen(tok));

			tok = strtok(NULL, "@");
			strncpy(url->auth.user, last, strlen(tok));

			last = tmp;
		}
	} else {
		tmp = last;
	}

	while(*tmp != ':' && *tmp != '/' && *tmp != 0) {
		tmp++;
	}

	if((tmp - last) < LIBNET_URL_SIZE_HOST) {
		strncpy(url->host, last, (tmp - last));
	}

	if(*tmp == ':') {
		last = ++tmp;
	
		while(*tmp != '/' && *tmp != 0) {
			tmp++;
		}

		if((tmp - last) < 6) { /* 0 - 65535 */
			char port[6] = {0};
			strncpy(port, last, (tmp - last));
			url->port = (uint16_t)atoi(port);
		}
	}

	if(*tmp == '/') {
		last = tmp;
		tmp = strstr(tmp, "?");

		if(tmp == NULL) {
			strncpy(url->path, last, LIBNET_URL_SIZE_PATH);
		} else if((tmp - last) < LIBNET_URL_SIZE_PATH) {
			strncpy(url->path, last, (tmp - last));
		}
	}

	if(tmp != NULL) {
		++tmp;
		strncpy(url->vars, tmp, LIBNET_URL_SIZE_VARS);
	}
}
