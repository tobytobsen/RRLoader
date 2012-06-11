#ifndef LIBNET_HTTP_RESPONSE_H_
#define LIBNET_HTTP_RESPONSE_H_

#include <types.h>

#include <net/http_def.h>

#include <net/hash_table.h>
#include <net/buffer.h>

struct http_ctx;
struct http_request;

typedef struct http_response {
	http_status_t	code;
	char			reason[LIBNET_HTTP_SIZE_REASON];

	htbl_t			header;
	buffer_t		body; // todo: a hash_table for dif. content entities would be better
} http_response_t;

void
http_response_parse(struct http_ctx *c, http_response_t *r, buffer_t *d);

// not needed now, we don't act as a server
//void
//http_response_create(struct http_ctx *c, struct http_request *rq, http_response_t *rs);

void
http_response_release(http_response_t *r);

#endif /* LIBNET_HTTP_RESPONSE_H_ */
