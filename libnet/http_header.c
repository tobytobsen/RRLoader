#include <net/net.h>
#include <net/http_header.h>

void
http_header_build(buffer_t *b, htbl_t *h) {
	if(b == NULL || h == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}
}

void
http_header_parse(htbl_t *h, buffer_t *b) {
	if(b == NULL || h == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}
}
