#ifndef LIBNET_HTTP_H_
#define LIBNET_HTTP_H_

#include <types.h>

#include <net/sockets.h>

#define LIBNET_HTTP_BUF_SIZE 256

typedef enum http_mime {
	LIBNET_HTTP_MIME_PLAIN = 0,
} http_mime_t;

typedef enum http_version {
	LIBNET_HTTP_V1 = 0,		// RFC 1945
	LIBNET_HTTP_V11,		// RFC 2616
	LIBNET_HTTP_V2,			// RFC ????
} http_version_t;

typedef enum http_method {
	LIBNET_HTTP_GET = 0,
	LIBNET_HTTP_POST,
	LIBNET_HTTP_HEAD,
} http_method_t;

typedef enum http_repsonse_code {
	LIBNET_HTTP_RES_OK = 200,

	LIBNET_HTTP_RES_NOT_FOUND = 404,

	LIBNET_HTTP_RES_FORBIDDEN = 503,
} http_response_code_t;

typedef struct http_form_ent {
	char name[LIBNET_HTTP_BUF_SIZE];
	char *body;
	
	http_mime_t mime;
} http_form_ent_t;

typedef struct http_form {
	char boundary[LIBNET_HTTP_BUF_SIZE];

	uint32_t entities;
	http_form_ent_t *entity;
} http_form_t;

typedef struct http_header_ent {
	char key[LIBNET_HTTP_BUF_SIZE];
	char value[LIBNET_HTTP_BUF_SIZE];
} http_header_ent_t;

typedef struct http_header {
	uint32_t entites;
	http_header_ent_t *entity;
} http_header_t;

typedef struct http_connection {
	socket_t *handle;
	char session[LIBNET_HTTP_BUF_SIZE];

	http_version_t version;
} http_con_t;

typedef struct http_request {
	http_header_t header;
	http_method_t method;

	http_form_t  *form;

	char path[LIBNET_HTTP_BUF_SIZE];
} http_request_t;

typedef struct http_response {
	http_response_code_t code;
	http_header_t header;

	char *body;
} http_response_t;

bool
http_connect(http_con_t __inout *h, const char *url);

void
http_disconnect(http_con_t __inout *h);

void
http_request_create(http_con_t __in *h, http_request_t __inout *r, http_version_t ver, http_method_t m, const char *path);

void
http_request_release(http_request_t __inout *r);

void
http_request_exec(http_con_t __in *h, http_request_t __in *r);

void
http_request_set_header(http_request_t __inout *r, const char *k, const char *v);

void
http_request_add_form_entity(http_request_t __inout *r, const char* name, const char *body, http_mime_t mime);


/*
	http_response_t res;
	http_request_t req;
	http_con_t con;

	if(false == http_connect(&con, "http://www.google.de")) {
		printf("unable to connect to remote host.\r\n");
		return -1;
	}

	http_request_create(&con, &req, LIBNET_HTTP_V11, LIBNET_HTTP_GET, "/");
	http_request_exec(&con, &req, &res);
	http_request_release(&res);

	printf("Request sent. Response: %d\r\n", res.code);

	http_disconnect(&con);
*/

#endif /* LIBNET_HTTP_H_ */
