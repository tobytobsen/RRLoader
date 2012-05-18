#ifndef LIBNET_URL_H_
#define LIBNET_URL_H_

#define LIBNET_URL_SIZE_SCHEME	16
#define LIBNET_URL_SIZE_AUTH	50
#define LIBNET_URL_SIZE_HOST	50
#define LIBNET_URL_SIZE_PATH	50
#define LIBNET_URL_SIZE_VARS 	256

#include <types.h>
//<user>:<password>@<host>:<port>/<url-path>
typedef struct url {
	char scheme[LIBNET_URL_SIZE_SCHEME];
	
	struct {
		char user[LIBNET_URL_SIZE_AUTH];
		char pass[LIBNET_URL_SIZE_AUTH];
	} auth;

	uint16_t port;

	char host[LIBNET_URL_SIZE_HOST];
	char path[LIBNET_URL_SIZE_PATH];
	char vars[LIBNET_URL_SIZE_VARS];
} url_t;

void
url_build(char __inout *dst, uint32_t len, url_t url);

void
url_parse(char __in *src, url_t __inout *url);

#endif /* LIBNET_URL_H_ */
