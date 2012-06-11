#ifndef LIBNET_HTTP_OPTIONS_H_
#define LIBNET_HTTP_OPTIONS_H_

#include <net/http_def.h>

#define LIBNET_HTTP_OPT_LONG(x)		(void*)(x)
#define LIBNET_HTTP_OPT_ADDR(x)		(void*)(&x)

struct http_ctx;

typedef enum http_opt {
	LIBNET_HTTP_OPT_VERSION,
	LIBNET_HTTP_OPT_METHOD,
	LIBNET_HTTP_OPT_AGENT,

	LIBNET_HTTP_OPT_PARAM, // length limitation: LIBNET_HTTP_HEADER_SIZE_VALUE
	LIBNET_HTTP_OPT_FORM_ADD,

	LIBNET_HTTP_OPT_FOLLOW,

	LIBNET_HTTP_OPT_CALLBACK_READ,

	LIBNET_HTTP_OPT_NONE,
} http_opt_t;

void
http_option_set(struct http_ctx *c, http_opt_t o, void *v);

void *
http_option_get_val(struct http_ctx *c, http_opt_t o);

bool
http_option_is_set(struct http_ctx *c, http_opt_t o);

/* internal */
char *
http_option_get_id(http_opt_t o);

#define OPTION_START(opt_var, opt)	uint32_t len = 0;\
									if(res == NULL) {\
										return 0;\
									}\
									if(opt_var >= LIBNET_HTTP_##opt##_UNDEFINED) {\
										opt_var = LIBNET_HTTP_##opt##_DEFAULT;\
									}

uint32_t
http_option_translate_version(http_ver_t v, void **res);

uint32_t
http_option_translate_method(http_method_t m, void **res);

#endif /* LIBNET_HTTP_OPTIONS_H_ */
