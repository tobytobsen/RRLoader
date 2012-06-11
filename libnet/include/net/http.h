/**
 * this file contains functions to handle http connections
 *
 * @file http.h
*/
#ifndef LIBNET_HTTP_H_
#define LIBNET_HTTP_H_

#include <types.h>

#include <net/http_def.h>

#include <net/url.h>
#include <net/socket.h>

#include <net/mutex.h>

#include <net/buffer.h>
#include <net/hash_table.h>

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

void
http_send(http_ctx_t *c, buffer_t *rs, uint32_t len);

void
http_read_header(http_ctx_t *c, buffer_t *rs);

/*
	simple get target:

	http_ctx_t ctx;

	http_connect(&ctx, "http://google.de");
	http_set_option(&ctx, LIBNET_HTTP_CALLBACK_READ, (void *)some_function);
	http_execute(&ctx);
	http_disconnect(&ctx);

*/

#endif /* LIBNET_HTTP_H_ */
