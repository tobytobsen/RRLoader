/**
 * @file url.h
 *
 * this header file contains functions to parse or build a URL
*/
#ifndef LIBNET_URL_H_
#define LIBNET_URL_H_

#define LIBNET_URL_SIZE_SCHEME	16
#define LIBNET_URL_SIZE_AUTH	50
#define LIBNET_URL_SIZE_HOST	50
#define LIBNET_URL_SIZE_PATH	50
#define LIBNET_URL_SIZE_VARS 	256

#include <types.h>

/**
 * contains every necessary information of an URL
 * which is constructed as followed:
 *
 * scheme://user:pass@host:port/path?vars
*/
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

/**
 * builds an url through the given struct
 *
 * @param dst destination string
 * @param len len of the destination string
 * @param url struct with information
*/
void
url_build(char __inout *dst, uint32_t len, url_t url);

/**
 * parses the string to fill the struct
 *
 * @param src url string
 * @param url target struct
*/
void
url_parse(const char __in *src, url_t __inout *url);

#endif /* LIBNET_URL_H_ */
