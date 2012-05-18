#ifndef LIBNET_SOCKET_H_
#define LIBNET_SOCKET_H_

#include <types.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)
# define WIN32
# include <winsock2.h>
#else
# define NIX
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/fcntl.h>
# include <netinet/in.h>
# include <netdb.h>
# include <unistd.h>
# include <arpa/inet.h>	

# include <errno.h>
#endif

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define LIBNET_SET_SIZE 64

typedef enum proto {
	LIBNET_PROTOCOL_TCP = 0,
	LIBNET_PROTOCOL_UDP,
	LIBNET_PROTOCOL_UNSUPPORTED,
} proto_t;

typedef enum encryption {
	LIBNET_ENC_SSL_V2 = 0,
	LIBNET_ENC_SSL_V3,
	LIBNET_ENC_TLS_V1,
	LIBNET_ENC_NONE,
} enc_t;

typedef enum socket_param {
	LIBNET_P_NONE = 0,

	LIBNET_P_TIMEOUT,

	LIBNET_P_ENC_AVOID_UNTRUSTED,
} socket_param_t;

typedef enum ip_ver {
	LIBNET_IPV4 = 0,
	LIBNET_IPV6,
	LIBNET_UNSUPPORTED_VER,
} ip_ver_t;

typedef struct socket {
	int 		handle;
	
	proto_t 	proto		: 2;
	ip_ver_t	ip_ver		: 2;
	enc_t		enc_type	: 2;

	uint8_t		_r0			: 2;

	union {
		struct sockaddr_in6 v6;
		struct sockaddr_in v4;
	} in;

	struct {
		struct {
			SSL_CTX 	*ctx;
			SSL 		*handle;
			SSL_METHOD	*method;
		} ssl;

		bool avoid_untrusted;
	} enc;

	struct timeval timeout;

	bool is_client;
} socket_t;

typedef struct socket_set {
	uint32_t cl_cur, cl_max;
	socket_t *client;
} socket_pool_t;

typedef union ip {
	uint32_t	v4;
	uint8_t 	_f[16];
} ip_t;

typedef uint16_t 	port_t;

#define socket_set_timeout(s, t) socket_set_param((s), LIBNET_P_TIMEOUT, ((void *)(&t)))

bool
socket_create_socket(socket_t __inout *s, proto_t p, ip_ver_t v);

void
socket_release_socket(socket_t __in *s);

bool
socket_connect(socket_t __inout *s, const char *address, port_t port);

bool
socket_listen(socket_t __inout *s, port_t port);

void
socket_disconnect(socket_t __inout *s);

bool
socket_accept(socket_t __in *listener, socket_pool_t __inout *set);

bool
socket_async_accept(socket_t __in *listener, socket_pool_t __inout *set);

uint32_t
socket_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len);

uint32_t
socket_async_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len);

void
socket_write(socket_t __in *s, uint8_t __in *buf, uint32_t len);

void
socket_async_write(socket_t __in *s, uint8_t __in *buf, uint32_t len);

bool
socket_set_encryption(socket_t __inout *s, enc_t enc, const char *f_cert, const char *f_key, const char *f_ca_cert);

void
socket_set_param(socket_t __inout *s, socket_param_t k, void *v);

void
socket_create_pool(socket_pool_t __inout *set);

void
socket_release_pool(socket_pool_t __in *set);

void
socket_pool_add_socket(socket_pool_t __inout *set, socket_t __in *s);

void
socket_pool_rem_socket(socket_pool_t __inout *set, socket_t __in *s);

uint32_t
socket_pool_get_size(socket_pool_t __in *set);

socket_t*
socket_pool_get_socket(socket_pool_t __in *set, uint32_t i);

#endif /* LIBNET_SOCKET_H_ */
