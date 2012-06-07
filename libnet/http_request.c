#include <net/net.h>
#include <net/http_request.h>
#include <net/http_options.h>
#include <net/http_header.h>

#include <string.h>

void
http_request_create(struct http_ctx *c, http_request_t *r) {
	char *key; void *val;
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

	/* default header fields */
	http_header_field_t he_cl = {"content-length", "0"}, // lazy :-)
						he_ct = {"content-type", "text/plain"};

	htbl_insert_copy(&r->header, he_cl.key, (void *)&he_cl, sizeof(he_cl));
	htbl_insert_copy(&r->header, he_ct.key, (void *)&he_ct, sizeof(he_ct));

	/* body */
	// do something

	/* calc content length */
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
