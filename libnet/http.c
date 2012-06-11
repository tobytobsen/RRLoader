#include <net/net.h>

#include <net/http.h>
#include <net/http_options.h>
#include <net/http_header.h>
#include <net/http_request.h>
#include <net/http_response.h>

#include <stdlib.h>
#include <string.h>

/* HTTP - CONNECTION */
bool
http_connect(http_ctx_t __inout *c, const char __in *url) {
	bool ssl = false;

	if(c == 0 || url == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	/* parse url */
	memset(c, 0, sizeof(http_ctx_t));
	url_parse(url, &c->url);

	/* if scheme is https, then we're about to use ssl.
		ssl usually requires an alternative port,
		so check for it or set it to 443 (default) if
		no port is set */
	ssl = !strcmp(c->url.scheme, "https") ? true : false;

	if(c->url.port == 0) {
		c->url.port = ssl ? LIBNET_HTTPS_PORT_DEF : LIBNET_HTTP_PORT_DEF;
	}

	/* create socket, set specific options and finally connect */
	if(false == socket_create_socket(&c->socket, LIBNET_PROTOCOL_TCP, LIBNET_IPV4)) {
		return false;
	}

	if(ssl == true) {
		if(false == socket_set_encryption(&c->socket, LIBNET_ENC_SSL_V3, NULL, NULL, NULL)) {
			http_disconnect(c);
			return false;
		}
	}

	if(false == socket_connect(&c->socket, c->url.host, c->url.port)) {
		socket_release_socket(&c->socket);
		return false;
	}

	/* initiate the rest */
	htbl_create(&c->options, 0);

	/* set default options */
	http_option_set(c, 
		LIBNET_HTTP_OPT_VERSION, 
		LIBNET_HTTP_OPT_LONG(LIBNET_HTTP_VER_DEFAULT));

	http_option_set(c, 
		LIBNET_HTTP_OPT_METHOD, 
		LIBNET_HTTP_OPT_LONG(LIBNET_HTTP_METHOD_DEFAULT));

	http_option_set(c,
		LIBNET_HTTP_OPT_FOLLOW,
		LIBNET_HTTP_OPT_LONG(false));

	http_option_set(c,
		LIBNET_HTTP_OPT_AGENT,
		LIBNET_HTTP_AGENT_DEFAULT);

	return true;
}

void
http_disconnect(http_ctx_t __inout *c) {
	if(c == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	/* release socket and so on */
	socket_disconnect(&c->socket);
	socket_release_socket(&c->socket);

	htbl_release(&c->options);
}

void
http_execute(http_ctx_t *c) {
	http_request_t req;
	http_response_t res;
	buffer_t req_s, res_s;
	char buf[LIBNET_HTTP_SIZE_CHUNK] = {0};
	uint32_t len = 0;

	if(c == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	/* build request */
	http_request_create(c, &req);
	buffer_create(&req_s, LIBNET_BM_FILE);
	buffer_create(&res_s, LIBNET_BM_FILE);

	/* serialize request to send */
	len = http_request_serialize(&req, &req_s);

	/* write / read */
	http_send(c, &req_s, len);
	http_read_header(c, &res_s);

	/* request sent, response read. 
		handle response */

	http_response_parse(c, &res, &res_s);
	/* 
		content handling here
	*/
	if(http_option_is_set(c, LIBNET_HTTP_OPT_CALLBACK_READ)) {
		http_cb_read_t cb_r = http_option_get_val(c, LIBNET_HTTP_OPT_CALLBACK_READ);
		uint32_t total = buffer_size(&res.body), i = 0;

		buffer_seek(&res.body, 0);

		while(i < total) {
			i += buffer_read(&res.body, buf, LIBNET_HTTP_SIZE_CHUNK);
			cb_r(buf, LIBNET_HTTP_SIZE_CHUNK);
		}
	}

	http_response_release(&res);

	/* done. release */
	buffer_release(&req_s);
	buffer_release(&res_s);

	http_request_release(&req);
}

void
http_send(http_ctx_t *c, buffer_t *rs, uint32_t len) {
	uint32_t o = 0, len_read;
	char buf[LIBNET_HTTP_SIZE_CHUNK] = {0};

	if(c == NULL || rs == NULL || len == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	while(o < len) {
		len_read += buffer_read(rs, buf, LIBNET_HTTP_SIZE_CHUNK);
		socket_write(&c->socket, buf, len_read);

		o += len_read;
	}
}

void
http_read_header(http_ctx_t *c, buffer_t *rs) {
	char buf[LIBNET_HTTP_SIZE_CHUNK] = {0};
	uint32_t i=0;

	if(c == NULL || rs == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	buffer_seek(rs, 0);

	while(0 != socket_read(&c->socket, buf+i, 1)) {
		buffer_write(rs, 1, buf+i++, 1);

		if(i == LIBNET_HTTP_SIZE_CHUNK) {
			i = 0;
		}

		if(strstr(buf, LIBNET_HTTP_EOH)) {
			break;
		}
	}
}

