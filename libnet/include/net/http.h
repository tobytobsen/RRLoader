#ifndef LIBNET_HTTP_H_
#define LIBNET_HTTP_H_

#include <types.h>

#include <net/url.h>
#include <net/socket.h>

#define LIBNET_HTTP_SIZE_BUF 		256
#define LIBNET_HTTP_SIZE_REQ		4096

#define LIBNET_HTTP_PORT_DEF		80
#define LIBNET_HTTPS_PORT_DEF		443

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
	LIBNET_HTTP_RES_CREATED,
	LIBNET_HTTP_RES_ACCEPTED,
	LIBNET_HTTP_RES_NO_CONTENT = 204,

	LIBNET_HTTP_RES_MOVED_PERM = 301,
	LIBNET_HTTP_RES_MOVED_TEMP,

	LIBNET_HTTP_RES_BAD_REQ = 400,
	LIBNET_HTTP_RES_UNAUTHORIZED,
	LIBNET_HTTP_RES_FORBIDDEN = 403,
	LIBNET_HTTP_RES_NOT_FOUND,

	LIBNET_HTTP_RES_SRV_ERROR = 500,
	LIBNET_HTTP_RES_NOT_IMPLEMENTED,
	LIBNET_HTTP_RES_BAD_GATEWAY,
	LIBNET_HTTP_RES_SVC_UNAVAILABLE,
} http_response_code_t;

typedef struct http_form_ent {
	char name[LIBNET_HTTP_SIZE_BUF];
	char *body;
	
	http_mime_t mime;
} http_form_ent_t;

typedef struct http_form {
	char boundary[LIBNET_HTTP_SIZE_BUF];

	uint32_t entities;
	http_form_ent_t *entity;
} http_form_t;

typedef struct http_header_ent {
	char key[LIBNET_HTTP_SIZE_BUF];
	char value[LIBNET_HTTP_SIZE_BUF];
} http_header_ent_t;

typedef struct http_header {
	uint32_t entites;
	http_header_ent_t *entity;
} http_header_t;

typedef struct http_connection {
	socket_t handle;
	char session[LIBNET_HTTP_SIZE_BUF];

	http_version_t version;
	struct url url;
} http_con_t;

typedef struct http_request {
	http_header_t header;
	http_method_t method;

	http_form_t  *form;

	char path[LIBNET_HTTP_SIZE_BUF];
} http_request_t;

typedef struct http_response {
	http_response_code_t code;
	http_header_t header;

	char *body;
} http_response_t;

bool
http_connect(http_con_t __inout *h, const char __in *url);

void
http_disconnect(http_con_t __inout *h);

void
http_request_create(http_con_t __in *h, http_request_t __inout *r, http_version_t ver, http_method_t m, const char __in *path);

void
http_request_release(http_request_t __inout *r);

void
http_request_exec(http_con_t __in *h, http_request_t __in *req, http_response_t __inout *res);

void
http_request_set_header(http_request_t __inout *r, const char __in *k, const char __in *v);

void
http_request_add_form_entity(http_request_t __inout *r, const char __in *name, const char __in *body, http_mime_t mime);


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
