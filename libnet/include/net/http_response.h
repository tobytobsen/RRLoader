#ifndef LIBNET_HTTP_RESPONSE_H_
#define LIBNET_HTTP_RESPONSE_H_

#include <types.h>

#include <net/hash_table.h>
#include <net/buffer.h>

typedef enum http_response_code {
	LIBNET_HTTP_RC_OK = 200,
} http_res_code_t;

typedef struct http_response {
	http_res_code_t code;
	htbl_t			header;
	buffer_t		body;
} http_response_t;

#endif /* LIBNET_HTTP_RESPONSE_H_ */
