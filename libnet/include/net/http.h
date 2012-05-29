/**
 * this file contains functions to handle http connections
 *
 * @file http.h
*/
#ifndef LIBNET_HTTP_H_
#define LIBNET_HTTP_H_

#include <types.h>

#include <net/url.h>
#include <net/socket.h>

#include <net/mutex.h>

#include <net/buffer.h>
#include <net/hash_table.h>

#define LIBNET_HTTP_SIZE_BUF 		256
#define LIBNET_HTTP_SIZE_REQ		4096

#define LIBNET_HTTP_PORT_DEF		80
#define LIBNET_HTTPS_PORT_DEF		443

#define LIBNET_HTTP_DEL				"\r\n"

typedef struct http_ctx {
	socket_t	socket;
	url_t		url;
	htbl_t		options;
	htbl_t		header_general;
} http_ctx_t;

bool
http_connect(http_ctx_t *c, const char *url);

void
http_disconnect(http_ctx_t *c);

void
http_execute(http_ctx_t *c);

#endif /* LIBNET_HTTP_H_ */
