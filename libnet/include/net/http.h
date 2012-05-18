#ifndef LIBNET_HTTP_H_
#define LIBNET_HTTP_H_

#include <types.h>

#include <net/sockets.h>

typedef enum http_mime {
	LIBNET_HTTP_MIME_PLAIN = 0,
} http_mime_t;

typedef enum http_version {
	LIBNET_HTTP_V1 = 0,		// RFC 1945
	LIBNET_HTTP_V11,		// RFC 2616
	LIBNET_HTTP_V2,			// RFC ????
} http_version_t;

typedef struct http_form_ent {
	char *name;
	char *data;
	
	http_mime_t mime;
} http_form_ent_t;

typedef struct http_form {
	char *boundary;

	uint32_t entities;
	http_form_ent_t *entity;
} http_form_t;

typedef struct http_header_ent {
	char *key;
	char *value;
} http_header_ent_t;

typedef struct http_header {
	uint32_t entites;
	http_header_ent_t *entity;
} http_header_t;

typedef struct http_connection {
	socket_t *handle;
	char *session;

	http_version_t version;
} http_con_t;

typedef struct http_request {
	http_header_t *header;

	char *method;
	char *path;
} http_request_t;

bool
http_connect(http_con_t __inout *h, const char *url);

void
http_disconnect(http_con_t __inout *h);


void
http_create_form(http_form_t *form);

void
http_release_form(http_form_t *form);

#endif /* LIBNET_HTTP_H_ */
