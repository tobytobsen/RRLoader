#include <net/net.h>
#include <net/http.h>
#include <net/http_response.h>
#include <net/http_header.h>

void
http_response_parse/*_and_read*/(struct http_ctx *c, http_response_t *r, buffer_t *d) {
	char buf[LIBNET_HTTP_SIZE_CHUNK] = {0}, *tok;
	uint32_t len, i = 0, chunk_size = 0;
	bool chunked = false;
	http_header_field_t *hf_e;

	if(c == NULL || r == NULL || d == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	htbl_create(&r->header, 0);
	buffer_create(&r->body, LIBNET_BM_FILE);

	len = buffer_size(d);
	buffer_seek(d, 0);

	/* read status line */
	while(len > 0) {
		buffer_read(d, buf + i, 1);
		
		if(strstr(buf, LIBNET_HTTP_DEL)) {
			break;
		}

		++i; --len;
	}

	//printf("status line: %s", buf);

	/* do something with the http version */

	/* get status code */
	tok = strstr(buf, " ") + 1;
	sprintf(buf, "%s", tok);
	sscanf(buf, "%3d", &r->code);

	/* get reason */
	i = strcspn(buf+4, LIBNET_HTTP_DEL) + 1;

	if(i > LIBNET_HTTP_SIZE_REASON) {
		i -= LIBNET_HTTP_SIZE_REASON; // LIBNET_HTTP_DEL;
	}

	snprintf(r->reason, i, "%s", buf+4);

	/* now parse header */
	http_header_parse(&r->header, d);

	/* chunked? */
	hf_e = (http_header_field_t *)htbl_get(&r->header, "transfer-encoding");
	chunked = (!strcmp(hf_e->value, "chunked") ? true : false);

	uint32_t get_chunk_size(socket_t *s) {
		uint32_t i = 0, size;
		char buf[LIBNET_HTTP_SIZE_CHUNK] = {0};

		while(!strstr(buf, LIBNET_HTTP_DEL)) {
			if(0 == socket_read(s, buf+i++, 1)) {
				return 0;
			}
		}

		if(i >= 3) {
			buf[i-2] = 0;
		} else {
			return 0;
		}

		sscanf(buf, "%x", &size);
		return size;
	}

	/* read chunked till end */
	if(chunked == true) {
		chunk_size = get_chunk_size(&c->socket);
		
		if(chunk_size == 0) {
			/* crit. error: 
				transfer is chunked, but the chunk size is unknown
			*/
			return;
		}

		do {
			//printf("chunk_size: %x\n", chunk_size);

			while(chunk_size > 0) {
				//printf("bytes to read: %x\n", chunk_size);

				i = (chunk_size > LIBNET_HTTP_SIZE_CHUNK ? LIBNET_HTTP_SIZE_CHUNK : chunk_size);
				i = socket_read(&c->socket, buf, i);

				if(i == 0) {
					break;
				}

				buffer_write(&r->body, 1, buf, i);

				chunk_size -= i;
			}
		} while(chunk_size = get_chunk_size(&c->socket) > 0);
	}
}

void
http_response_release(http_response_t *r) {
	if(r == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	htbl_release(&r->header);
	buffer_release(&r->body);
}
