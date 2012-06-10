#ifndef LIBNET_HTTP_REQUEST_H_
#define LIBNET_HTTP_REQUEST_H_

#include <types.h>

#include <net/hash_table.h>
#include <net/buffer.h>

struct http_ctx;

#define LIBNET_HTTP_SIZE_REQLINE	256

typedef struct http_request {
	char		line[LIBNET_HTTP_SIZE_REQLINE];
	htbl_t 		header; // general + request + entity header
	buffer_t	body;
} http_request_t;

void
http_request_create(struct http_ctx *c, http_request_t *r);

void
http_request_release(http_request_t *r);

uint32_t
http_request_serialize(http_request_t *r, buffer_t *b);

#endif /* LIBNET_HTTP_REQUEST_H_ */
