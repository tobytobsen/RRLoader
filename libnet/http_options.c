#include <net/net.h>
#include <net/http.h>
#include <net/http_options.h>

#include <stdlib.h>
#include <string.h>

void
http_option_set(http_ctx_t *c, http_opt_t o, void *v) {
	void *opt_val = NULL;
	char *opt_id = NULL;
	uint32_t opt_len = 0;
	bool copy = true;

	if(c == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	opt_id = http_option_get_id(o);

	switch(o) {
		case LIBNET_HTTP_OPT_VERSION: {
			if(v == NULL) {
				break;
			}

			opt_len = http_option_translate_version((uint32_t)(v), &opt_val);
		} break;

		case LIBNET_HTTP_OPT_METHOD: {
			if(v == NULL) {
				break;
			}

			opt_len = http_option_translate_method((uint32_t)(v), &opt_val);
		} break;

		case LIBNET_HTTP_OPT_AGENT: {
			if(v == NULL) {
				break;
			}

			opt_len = strlen((char *)v);
			opt_val = v;
		} break;

		case LIBNET_HTTP_OPT_FOLLOW: {
			copy = false;

			if(((bool)v) != true && ((bool)v) != false) {
				break;
			}

			opt_len = 1;
			opt_val = v;
		} break;

		case LIBNET_HTTP_OPT_CALLBACK_READ: {
			copy = false;

			opt_len = 4;
			opt_val = v;
		} break;

		default: return;
	}

	if(opt_len > 0) {
		/*printf("inserting '%s' -> '%s' of size %d %s\n",
			opt_id, opt_val, opt_len,
			copy == true ? "(copy)": "");
*/
		if(copy == true) {
			htbl_insert_copy(&c->options, opt_id, opt_val, opt_len);
		} else {
			htbl_insert(&c->options, opt_id, opt_val);
		}
	}
}

void *
http_option_get_val(struct http_ctx *c, http_opt_t o) {
	char *opt_id = NULL;
	void *opt = NULL;

	if(c == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}

	opt_id = http_option_get_id(o);
	opt = htbl_get(&c->options, opt_id);

	return opt;
}

char *
http_option_get_id(http_opt_t o) {
	switch(o) {
		case LIBNET_HTTP_OPT_VERSION:
			return "version";

		case LIBNET_HTTP_OPT_AGENT: 
			return "user-agent";

		case LIBNET_HTTP_OPT_METHOD:
			return "method";

		case LIBNET_HTTP_OPT_FOLLOW:
			return "follow";

		case LIBNET_HTTP_OPT_CALLBACK_READ:
			return "cbt_read";

		default: 
			return NULL;
	}
}

uint32_t
http_option_translate_version(http_ver_t v, void **res) {
	OPTION_START(v, VER);

	switch(v) {
		case LIBNET_HTTP_VER_10: {
			*res = (void *)"HTTP/1.0";
		} break;
		
		case LIBNET_HTTP_VER_11: {
			*res = (void *)"HTTP/1.1";
		} break;
		
		case LIBNET_HTTP_VER_20: {
			*res = (void *)"HTTP/2.0";
		} break;
	}

	len = strlen(*(char **)res);
	return len;
}

uint32_t
http_option_translate_method(http_method_t m, void **res) {
	OPTION_START(m, METHOD);

	switch(m) {
		case LIBNET_HTTP_METHOD_GET:
			*res = (void *)"GET";

		case LIBNET_HTTP_METHOD_PUT:
			*res = (void *)"PUT";

		case LIBNET_HTTP_METHOD_POST:
			*res = (void *)"POST";

		case LIBNET_HTTP_METHOD_HEAD:
			*res = (void *)"HEAD";

		case LIBNET_HTTP_METHOD_OPTIONS:
			*res = (void *)"OPTIONS";

		case LIBNET_HTTP_METHOD_TRACE:
			*res = (void *)"TRACE";

		case LIBNET_HTTP_METHOD_CONNECT:
			*res = (void *)"CONNECT";

		case LIBNET_HTTP_METHOD_DELETE:
			*res = (void *)"DELETE";
	}

	len = strlen(*(char **)res);
	return len;
}
