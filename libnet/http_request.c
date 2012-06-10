#include <net/net.h>
#include <net/http.h>
#include <net/http_request.h>
#include <net/http_options.h>
#include <net/http_header.h>

#include <string.h>

#define HEADER_INSERT(tbl, _key, _val)\
	strncpy(hf.key, (_key), LIBNET_HTTP_HEADER_SIZE_KEY);\
	strncpy(hf.value, (_val), LIBNET_HTTP_HEADER_SIZE_VALUE);\
	htbl_insert_copy((tbl), (_key), (void *)&hf, sizeof(hf));

void
http_request_create(struct http_ctx *c, http_request_t *r) {
	http_header_field_t hf;
	char *key; void *val;
	char buf[LIBNET_HTTP_HEADER_SIZE_VALUE] = {0};
	uint32_t len=0;
	http_header_field_t he;
	uint32_t i=0;

	http_opt_t ogh[] = { /* general header */
		LIBNET_HTTP_OPT_AGENT,

		LIBNET_HTTP_OPT_NONE,
	};

	if(c == NULL || r == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	htbl_create(&r->header, 0);
	buffer_create(&r->body, LIBNET_BM_MEMORY);

	/* get header information out of the options */
	while(ogh[i] != LIBNET_HTTP_OPT_NONE) {
		val = http_option_get_val(c, ogh[i]);
		key = http_option_get_id(ogh[i]);

		if(val != NULL && key != NULL) {
			strncpy(he.key, key, LIBNET_HTTP_HEADER_SIZE_KEY);
			strncpy(he.value, (char *)val, LIBNET_HTTP_HEADER_SIZE_VALUE);

			htbl_insert_copy(&r->header, key, (void *)&he, sizeof(he));
		}

		++i;
	}

	/* general header fields */
	HEADER_INSERT(&r->header, "connection", "close");

	/* request header fields */
	/* (most parts should be in options) */
	HEADER_INSERT(&r->header, "host", c->url.host);

	/* body */
	if(true == http_option_is_set(c, LIBNET_HTTP_OPT_PARAM)) {
		char *param = http_option_get_val(c, LIBNET_HTTP_OPT_PARAM);
		len = strlen(param),
		buffer_write(&r->body, 1, param, len);
	}

	/* entity header fields */
	if(len > 0) {
		snprintf(buf, LIBNET_HTTP_HEADER_SIZE_VALUE-1, "%d", len);
		HEADER_INSERT(&r->header, "content-length", buf);
		HEADER_INSERT(&r->header, "content-type", "text/plain");
	}

	/* request line */
	snprintf(r->line, LIBNET_HTTP_SIZE_REQLINE-1, "%s %s %s\r\n",
		(char *)http_option_get_val(c, LIBNET_HTTP_OPT_METHOD),
		c->url.path,
		(char *)http_option_get_val(c, LIBNET_HTTP_OPT_VERSION));
}

void
http_request_release(http_request_t *r) {
	if(r == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	htbl_release(&r->header);
	buffer_release(&r->body);
}

uint32_t
http_request_serialize(http_request_t *r, buffer_t *b) {
	uint32_t len = 0;

	if(r == NULL || b == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	/* get request line */
	buffer_write(b, 1, r->line, strlen(r->line));

	/* get request header */
	http_header_build(b, &r->header);

	/* get size of request */
	if(buffer_size(&r->body) > 0) {
		buffer_write(b, 1, buffer_get(&r->body), buffer_size(&r->body));
	}

	len = buffer_size(b);
	buffer_seek(b, 0);

	return len;
}
