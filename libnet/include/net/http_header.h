#ifndef LIBNET_HTTP_HEADER_H_
#define LIBNET_HTTP_HEADER_H_

#include <types.h>

#include <net/hash_table.h>
#include <net/buffer.h>

void
http_header_build(buffer_t *b, htbl_t *h);

void
http_header_parse(htbl_t *h, buffer_t *b);

#endif /* LIBNET_HTTP_HEADER_H_ */
