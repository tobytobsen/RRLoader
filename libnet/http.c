#include <net/net.h>
#include <net/http.h>

#include <string.h>
#include <ctype.h>

uint32_t
get_chunk_size(http_con_t *h) {
	char buf[32] = {0}, buff[32], *b;
	uint32_t o = 0, len = 0;

	if(h == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return 0;
	}

	while(NULL == strstr(buf, LIBNET_HTTP_DEL) && o < 32) {
		socket_read(&h->handle, buf+o++, 1);
	}

	sprintf(buf, "%d", (int)strtol(buf, NULL, 16));
	return atoi(buf);
}

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

void
build_request(http_con_t *h, http_request_t *req, buffer_t *buf) {
	if(h == 0 || req == 0 || buf == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	buffer_write_formatted(buf, "%s %s %s%s", 
		get_method_str(req->method),
		req->path,
		get_version_str(h->version),
		LIBNET_HTTP_DEL);

	if(htbl_empty(&req->header.tbl) == false) {
		uint32_t i = 0;
		http_header_ent_t *e = NULL;

		while(NULL != htbl_enumerate(&req->header.tbl, &i, NULL, (void **)&e)) {
			buffer_write_formatted(buf, "%s: %s%s",
				e->key, e->value,
				LIBNET_HTTP_DEL);
		}
	}

	buffer_write_formatted(buf, "%s", LIBNET_HTTP_DEL);

	//printf("Request: %s\n", buffer_get(buf));
}

void
parse_response(http_con_t *h, http_response_t *res, const char *buf, uint32_t len) {
	char *tmp;
	uint32_t tmplen;

	if(h == 0 || res == 0 || buf == 0 || len == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	tmp = (char*)get_version_str(h->version);
	tmplen = strlen(tmp);

	if(buf != strstr(buf, tmp)) {
		//// printf("%d\n", h->version);
		return; // malformed
	}

	tmp = ((char *)buf) + tmplen + 1;
	res->sig = LIBNET_SIG_HTTP_RESPONSE;
	res->code = (http_response_code_t)(atoi(tmp));

	htbl_create(&res->header.tbl, 0);

	tmp = strtok((char *)buf, LIBNET_HTTP_DEL);
	
	while(NULL != (tmp = strtok(NULL, LIBNET_HTTP_DEL))) {
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
http_connect(http_con_t __inout *h, const char __in *url, http_version_t ver) {
	url_t purl;
	bool ssl = false;

	if(h == 0 || url == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	if(ver < LIBNET_HTTP_V1 || ver > LIBNET_HTTP_V2) {
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

	h->version = ver;

	return true;
}

void
http_disconnect(http_con_t __inout *h) {
	if(h == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	socket_disconnect(&h->handle);
	socket_release_socket(&h->handle);
}

/*void
http_rules_set_rule(http_rules_t *rules, http_rule_t rule, void *val) {

}

void
http_rules_remove_rule(http_rules_t *rules, http_rule_t rule) {

}*/

void
http_request_create(http_con_t __in *h, http_request_t __inout *r, http_method_t m, const char __in *path) {
	static uint32_t rid = 0;

	if(h == 0 || r == 0 || path == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(m < LIBNET_HTTP_GET || m > LIBNET_HTTP_HEAD) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	memset(r, 0, sizeof(http_request_t));
	
	r->id = ++rid;
	r->sig = LIBNET_SIG_HTTP_REQUEST;
	r->method = m;

	strncpy(r->path, path, LIBNET_HTTP_SIZE_BUF);

	htbl_create(&r->header.tbl, 0);
	htbl_create(&r->form.tbl, 0);

	http_header_set_kv_pair(r, "host", h->url.host);
}

void
http_request_release(http_request_t __inout *r) {
	if(r == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	htbl_release(&r->header.tbl);
	htbl_release(&r->form.tbl);
	htbl_release(&r->res.header.tbl);

	buffer_release(&r->res.body);
}

void
http_request_set_callback(http_request_t *req, http_callback_t cbt, void *fp) {
	if(cbt >= LIBNET_HTTP_CBT_NONE || fp == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	switch(cbt) {
		case LIBNET_HTTP_CBT_READ: {
			req->cb.read = (http_cb_read_t)fp;
		} break;
	}
}

/* doesn't seem to read everything.. */
void
http_request_exec(http_con_t __in *h, http_request_t __in *req, http_response_t __inout *res) {
	char c, chunk[LIBNET_HTTP_SIZE_REQ] = {0}, *tmp;
	int len, to_read = 0;

	if(h == 0 || req == 0 || res == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	mutex_acquire(&h->mtx_re);

	memset(res, 0, sizeof(http_response_t));
	htbl_create(&res->header.tbl, 0);

	if(false == buffer_create(&res->body, LIBNET_BM_MEMORY)) {
		buffer_release(&res->body);
		mutex_release(&h->mtx_re);
		return;
	}

	build_request(h, req, &res->body);
	socket_write(&h->handle, buffer_get(&res->body), buffer_size(&res->body));
	buffer_clear(&res->body, 0);

	while(0 != socket_read(&h->handle, &c, 1)) {
		buffer_write(&res->body, 1, &c, 1);

		if(strstr(buffer_get(&res->body), LIBNET_HTTP_EOH)) {
			break;
		}

		if(buffer_size(&res->body) >= LIBNET_HTTP_SIZE_REQ) {
			mutex_release(&h->mtx_re);
			buffer_release(&res->body);
			return; // failure, header is supposed to be <4096 bytes
		}
	}

	parse_response(h, res, buffer_get(&res->body), buffer_size(&res->body));
	buffer_clear(&res->body, 0);

	/* check the way the server sends data */
	if(NULL != (tmp = (char *)http_header_get_value_by_name(res, "transfer-encoding"))) {
		if(!strcmp(tmp, "chunked")) {
			to_read = get_chunk_size(h);
		}
	} else if(NULL != (tmp = (char *)http_header_get_value_by_name(res, "content-length"))) {
		to_read = atoi(tmp);
	} else {
		buffer_release(&res->body);
		mutex_release(&h->mtx_re);
		return; // not supported
	}

	// printf("chunk size: %d\r\n", to_read);

	while(/*true == socket_is_readable(&h->handle)*/to_read > 0) {
		len = LIBNET_HTTP_SIZE_REQ < to_read ? LIBNET_HTTP_SIZE_REQ : to_read;
		len = socket_read(&h->handle, chunk, len);

		to_read -= len;

		if(req->cb.read == NULL) {
			buffer_write(&res->body, 1, chunk, len);
		} else {
			req->cb.read(req->id, chunk, len);
		}

		/* check, if there is data to read */
		if(to_read <= 0 && socket_is_readable(&h->handle)) {
			to_read = get_chunk_size(h);

			if(to_read == 0) {
				break;
			}
		}
	}

	memcpy(&req->res, res, sizeof(http_response_t));
	buffer_create_from_buffer(&req->res.body, &res->body);
	buffer_release(&res->body);

	h->last_request = req;
	mutex_release(&h->mtx_re);
}

void
http_header_set_kv_pair(void __inout *r, char __in *k, char __in *v) {
	http_header_t *header = NULL;

	char *strtolower(char *str) {
		uint16_t i=0;
		static char lstr[LIBNET_HTTP_SIZE_REQ] = {0};
		char *tmp = (char *)lstr; // tmp as str directly will result in a bus error 

		if(strlen(str) >= LIBNET_HTTP_SIZE_REQ) {
			return NULL;
		}

		for(i=0; i<LIBNET_HTTP_SIZE_REQ || str[i] == 0; i++) {
			lstr[i] = tolower(str[i]);
		}

		return lstr;
	}

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

	http_header_ent_t ent = {0};

	strncpy(ent.key, strtolower(k), LIBNET_HTTP_SIZE_BUF);
	strncpy(ent.value, strtolower(v), LIBNET_HTTP_SIZE_BUF);

	htbl_insert_copy(&header->tbl, (uint8_t *)ent.key, &ent, sizeof(http_header_ent_t));
}

const char *
http_header_get_value_by_name(void __inout *r, const char __in *name) {
	http_header_t *h = NULL;
	http_header_ent_t *e = NULL;

	if(r == 0 || name == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}	

	switch(*(uint8_t *)(r)) {
		case LIBNET_SIG_HTTP_RESPONSE: {
			h = &((http_response_t *)(r))->header;
		} break;

		case LIBNET_SIG_HTTP_REQUEST: {
			h = &((http_request_t *)(r))->header;
		} break;

		default: return NULL;
	}

	e = (http_header_ent_t *)htbl_get(&h->tbl, (uint8_t *)name);
	return (e != NULL) ? e->value : NULL;
}

void
http_request_add_form_entity(http_request_t __inout *r, const char __in *name, const char __in *body, http_mime_t mime) {
	
}

