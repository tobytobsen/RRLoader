#include <net/net.h>
#include <net/http.h>
#include <net/http_header.h>

#include <ctype.h>

void
http_header_build(buffer_t *b, htbl_t *h) {
	uint32_t i=0;
	http_header_field_t *e = NULL;

	if(b == NULL || h == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	while(NULL != htbl_enumerate(h, &i, NULL, (void **)&e, NULL)) {
		buffer_write_formatted(b, "%s: %s%s", 
			e->key, e->value, LIBNET_HTTP_DEL);
	}

	buffer_write(b, 1, LIBNET_HTTP_DEL, 2);
}

void
http_header_parse(htbl_t *h, buffer_t *b) {
	int eoh = 0;
	char l=0, c = ' ', *t;
	http_header_field_t e = {0};

	if(b == NULL || h == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	t = &e.key;

	buffer_seek(b, 0);

	while(eoh < 4 && c != 0) {
		int len = buffer_read(b, &c, 1);

		if(c == ':') {
			t = &e.value;
		} else if(isalpha(c) || isdigit(c) || ispunct(c)) {
			*t++ = tolower(c);
		} else if(iscntrl(c)) {
			if(l != '\r' && l != '\n') {
				eoh = 0;
			}

			eoh++;

			if(l == '\r' && c == '\n') {
				htbl_insert_copy(h, e.key, (void *)&e, sizeof(http_header_field_t));

				t = &e.key;
				memset(&e, 0, sizeof(http_header_field_t));
			}
		}

		l = c;
	}
}
