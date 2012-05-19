#include <net/net.h>
#include <net/http.h>

#include <string.h>

const char *
get_method_str(http_method_t m) {
	switch(m) {
		case LIBNET_HTTP_GET: return "GET";
		case LIBNET_HTTP_POST: return "POST";
		case LIBNET_HTTP_HEAD: return "HEAD";
		default: return NULL;
	}
}

const char *
get_version_str(http_version_t v) {
	switch(v) {
		case LIBNET_HTTP_V1: return "HTTP/1.0";
		case LIBNET_HTTP_V11: return "HTTP/1.1";
		case LIBNET_HTTP_V2: return "HTTP/2.0";
	}
}

char *
build_request(http_con_t *h, http_request_t *req) {
	char *buf;

	if(h == 0 || req == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}

	buf = calloc(1, LIBNET_HTTP_SIZE_REQ);

	if(req->form != NULL) {
		// ..
	}

	snprintf(buf, LIBNET_HTTP_SIZE_REQ - 1, "%s %s %s\r\n", 
		get_method_str(req->method),
		req->path,
		get_version_str(h->version));

	if(req->header.entities > 0 && req->header.entity != 0) {
		uint32_t i=0;

		for(i=0; i<req->header.entities; i++) {
			snprintf(buf, LIBNET_HTTP_SIZE_REQ - 1, "%s%s: %s\r\n",
				buf, req->header.entity[i].key, req->header.entity[i].value);
		}
	}

	snprintf(buf, LIBNET_HTTP_SIZE_REQ - 1, "%s\r\n", buf);
	return buf;
}

void
parse_response(http_con_t *h, http_response_t *res, char *buf, uint32_t len) {
	char *tmp;
	uint32_t tmplen;

	if(h == 0 || res == 0 || buf == 0 || len == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	tmp = get_version_str(h->version);
	tmplen = strlen(tmp);

	if(buf != strstr(buf, tmp)) {
		return; // malformed
	}

	tmp = buf + tmplen + 1;
	res->sig = LIBNET_SIG_HTTP_RESPONSE;
	res->code = (http_response_code_t)(atoi(tmp));
	res->body = 0;

	tmp = strtok(buf, "\r\n");
	while(NULL != (tmp = strtok(NULL, "\r\n"))) {
		http_header_ent_t he = {0};
		char *del = strstr(tmp, ":") + 1;
		
		if(del == NULL) {
			break;
		}

		if((del - tmp) < LIBNET_HTTP_SIZE_BUF) {
			strncpy(he.key, tmp, (del - tmp) - 1);

			while(*del != 0  && *del != '\r' && *del == ' ') {
				del++;
			}

			strncpy(he.value, del, LIBNET_HTTP_SIZE_BUF);
			http_header_set_kv_pair(res, he.key, he.value);
		}
	}
}

bool
http_connect(http_con_t __inout *h, const char __in *url) {
	url_t purl;
	bool ssl = false;

	if(h == 0 || url == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	url_parse(url, &purl);
	
	if(false == socket_create_socket(&h->handle, LIBNET_PROTOCOL_TCP, LIBNET_IPV4)) {
		return false;
	}

	ssl = !strcmp(purl.scheme, "https") ? true : false;

	if(purl.port == 0) {
		purl.port = ssl ? LIBNET_HTTPS_PORT_DEF : LIBNET_HTTP_PORT_DEF;
	}

	if(false == socket_connect(&h->handle, purl.host, purl.port)) {
		socket_release_socket(&h->handle);
		return false;
	}

	if(ssl == true) {
		if(false == socket_set_encryption(&h->handle, LIBNET_ENC_SSL_V3, NULL, NULL, NULL)) {
			http_disconnect(h);
			return false;
		}
	}

	memcpy(&h->url, &purl, sizeof(url_t));

	return true;
}

void
http_disconnect(http_con_t __inout *h) {
	if(h == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	socket_disconnect(&h->handle);
	socket_release_socket(&h->handle);
}

void
http_request_create(http_con_t __in *h, http_request_t __inout *r, http_version_t ver, http_method_t m, const char __in *path) {
	if(h == 0 || r == 0 || path == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(ver < LIBNET_HTTP_V1 || ver > LIBNET_HTTP_V2) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(m < LIBNET_HTTP_GET || m > LIBNET_HTTP_HEAD) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	memset(r, 0, sizeof(http_request_t));
	
	r->sig = LIBNET_SIG_HTTP_REQUEST;
	r->method = m;

	strncpy(r->path, path, LIBNET_HTTP_SIZE_BUF);

	http_header_set_kv_pair(r, "host", h->url.host);
}

void
http_request_release(http_request_t __inout *r) {
	if(r == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(r->header.entity != NULL) {
		free(r->header.entity);
	}

	if(r->form != NULL) {
		if(r->form->entity != NULL) {
			free(r->form->entity);
		}

		free(r->form);
	}
}

void
http_request_exec(http_con_t __in *h, http_request_t __in *req, http_response_t __inout *res) {
	char *buf, chunk[LIBNET_HTTP_SIZE_REQ];
	uint32_t chunks = 1, o = 0;
	int len;

	if(h == 0 || req == 0 || res == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	buf = build_request(h, req);
	socket_write(&h->handle, buf, strlen(buf));

	/* todo: read until header end, then parse response and continue */
	while(0 != (len = socket_read(&h->handle, chunk, LIBNET_HTTP_SIZE_REQ))) {
		if(o >= (chunks * LIBNET_HTTP_SIZE_REQ)) {
			buf = realloc(buf, ++chunks * LIBNET_HTTP_SIZE_REQ);
			o = (chunks-1) * LIBNET_HTTP_SIZE_REQ;
		}

		strncpy(buf + o, chunk, LIBNET_HTTP_SIZE_REQ);
		o += LIBNET_HTTP_SIZE_REQ;
	}

	parse_response(h, res, buf, strlen(buf));
	free(buf);
}

void
http_header_set_kv_pair(void __inout *r, const char __in *k, const char __in *v) {
	http_header_ent_t *ent;
	http_header_t *header;

	if(r == 0 || k == 0 || v == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	switch(*(uint8_t *)(r)) {
		case LIBNET_SIG_HTTP_RESPONSE: {
			header = &((http_response_t *)(r))->header;
		} break;

		case LIBNET_SIG_HTTP_REQUEST: {
			header = &((http_request_t *)(r))->header;
		} break;

		default: return; // invalid signature
	}

	if(header->entity == 0) {
		header->entity = calloc(1, sizeof(http_header_ent_t));
	} else {
		header->entity = realloc(header->entity, 
			sizeof(http_header_ent_t) * (header->entities + 1));
	}

	if(header->entity == 0) {
		//libnet_error_set(LIBNET_E_MEM);
		return;
	}

	ent = &header->entity[header->entities++];
	strncpy(ent->key, k, LIBNET_HTTP_SIZE_BUF);
	strncpy(ent->value, v, LIBNET_HTTP_SIZE_BUF);
	ent->key[0] = tolower(ent->key[0]);
}

const char *
http_header_get_value_by_name(void __inout *r, const char __in *name) {
	http_header_t *h;
	uint32_t i;

	if(r == 0 || name == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}	

	switch(*(uint8_t *)(r)) {
		case LIBNET_SIG_HTTP_RESPONSE: {
			h = &((http_response_t *)(r))->header;
		} break;

		case LIBNET_SIG_HTTP_REQUEST: {
			h = &((http_request_t *)(r))->header;
		} break;
	}

	for(i=0; i<h->entities; i++) {
		if(!strcmp(h->entity[i].key, name)) {
			return h->entity[i].value;
		}
	}

	return NULL;
}

void
http_request_add_form_entity(http_request_t __inout *r, const char __in *name, const char __in *body, http_mime_t mime) {

}

