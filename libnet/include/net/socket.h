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
#endif

#define LIBNET_SET_SIZE 64

typedef enum socket_proto {
	LIBNET_PROTOCOL_TCP = 0,
	LIBNET_PROTOCOL_UDP,
	LIBNET_PROTOCOL_UNSUPPORTED,
} proto_t;

typedef enum ip_ver {
	LIBNET_IPV4 = 0,
	LIBNET_IPV6,
	LIBNET_UNSUPPORTED_VER,
} ip_ver_t;

typedef struct socket {
	int 		handle;
	
	proto_t 	proto		: 2;
	ip_ver_t	ip_ver		: 2;

	uint8_t		_r0			: 4;

	union {
		struct sockaddr_in6 v6;
		struct sockaddr_in v4;
	} in;

	struct timeval timeout;
} socket_t;

typedef struct socket_set {
	uint32_t cl_cur, cl_max;
	socket_t *client;
} socket_set_t;

typedef union ip {
	uint32_t	v4;
	uint8_t 	_f[16];
} ip_t;

typedef uint16_t 	port_t;

bool
socket_create_socket(socket_t __inout *s, proto_t p, ip_ver_t v);

void
socket_release_socket(socket_t __in *s);

void
socket_set_timeout(socket_t __inout *s, struct timeval t);

bool
socket_connect(socket_t __inout *s, const char *address, port_t port);

bool
socket_listen(socket_t __inout *s, port_t port);

void
socket_disconnect(socket_t __inout *s);

bool
socket_accept(socket_t __in *listener, socket_set_t __inout *set);

bool
socket_async_accept(socket_t __in *listener, socket_set_t __inout *set);

uint32_t
socket_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len);

uint32_t
socket_async_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len);

void
socket_write(socket_t __in *s, uint8_t __in *buf, uint32_t len);

void
socket_async_write(socket_t __in *s, uint8_t __in *buf, uint32_t len);

void
socket_create_set(socket_set_t __inout *set);

void
socket_release_set(socket_set_t __in *set);

void
socket_set_add_socket(socket_set_t __inout *set, socket_t __in *s);

void
socket_set_rem_socket(socket_set_t __inout *set, socket_t __in *s);

uint32_t
socket_set_get_client_amount(socket_set_t __in *set);

socket_t*
socket_set_get_client(socket_set_t __in *set, uint32_t i);

#endif /* LIBNET_SOCKET_H_ */
