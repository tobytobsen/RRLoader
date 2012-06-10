#ifndef LIBNET_HTTP_OPTIONS_H_
#define LIBNET_HTTP_OPTIONS_H_

#define LIBNET_HTTP_OPT_LONG(x)		(void*)(x)
#define LIBNET_HTTP_OPT_ADDR(x)		(void*)(&x)

#define LIBNET_HTTP_AGENT_DEFAULT	"libnet-api-http-mod"

struct http_ctx;

typedef enum http_ver {
	LIBNET_HTTP_VER_10 = 1,
	LIBNET_HTTP_VER_11,
	LIBNET_HTTP_VER_20,
	LIBNET_HTTP_VER_UNDEFINED,

	LIBNET_HTTP_VER_DEFAULT = LIBNET_HTTP_VER_11,	
} http_ver_t;

typedef enum http_method {
	LIBNET_HTTP_METHOD_GET = 1,
	LIBNET_HTTP_METHOD_PUT,
	LIBNET_HTTP_METHOD_POST,
	LIBNET_HTTP_METHOD_HEAD,
	LIBNET_HTTP_METHOD_OPTIONS,
	LIBNET_HTTP_METHOD_TRACE,
	LIBNET_HTTP_METHOD_CONNECT,
	LIBNET_HTTP_METHOD_DELETE,
	LIBNET_HTTP_METHOD_UNDEFINED,

	LIBNET_HTTP_METHOD_DEFAULT = LIBNET_HTTP_METHOD_GET,
} http_method_t;

typedef void (*http_cb_read_t) (char *data, uint32_t len);

typedef enum http_opt {
	LIBNET_HTTP_OPT_VERSION,
	LIBNET_HTTP_OPT_METHOD,
	LIBNET_HTTP_OPT_AGENT,

	LIBNET_HTTP_OPT_PARAM,
	//LIBNET_HTTP_OPT_FORM_ADD,

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
