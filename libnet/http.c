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
	buffer_t req_str;
	uint32_t len = 0;

	if(c == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	/* build request */
	http_request_create(c, &req);
	buffer_create(&req_str, LIBNET_BM_MEMORY);

	/* get request line */
	buffer_write(&req_str, 1, req.line, strlen(req.line));

	/* get request header */
	http_header_build(&req_str, &req.header);

	/* get size of request */
	if(buffer_size(&req.body) > 0) {
		buffer_write(&req_str, 1, buffer_get(&req.body), buffer_size(&req.body));
	}

	len = buffer_size(&req_str);
	buffer_seek(&req_str, 0);

	/* write / read */
	//printf("%s", buffer_get(&req_str));

	/* done. release */
	buffer_release(&req_str);
	http_request_release(&req);
}
