#ifndef LIBNET_HTTP_HEADER_H_
#define LIBNET_HTTP_HEADER_H_

#include <types.h>

#include <net/http_def.h>

#include <net/hash_table.h>
#include <net/buffer.h>

struct http_ctx;

typedef struct http_header_field {
	char key[LIBNET_HTTP_HEADER_SIZE_KEY];
	char value[LIBNET_HTTP_HEADER_SIZE_VALUE];
} http_header_field_t;

void
http_header_build(buffer_t *b, htbl_t *h);

void
http_header_parse(htbl_t *h, buffer_t *b);

#endif /* LIBNET_HTTP_HEADER_H_ */
