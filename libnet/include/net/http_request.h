#ifndef LIBNET_HTTP_REQUEST_H_
#define LIBNET_HTTP_REQUEST_H_

#include <types.h>

#include <net/hash_table.h>
#include <net/buffer.h>

struct http_ctx;

typedef struct http_request {
	htbl_t 		header; // general + request + entity header
	buffer_t	body;
} http_request_t;

void
http_request_create(struct http_ctx *c, http_request_t *r);

void
http_request_release(http_request_t *r);

#endif /* LIBNET_HTTP_REQUEST_H_ */
