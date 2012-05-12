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
} socket_t;

typedef union ip {
	uint32_t	v4;
	uint8_t 	_f[16];
} ip_t;

typedef uint16_t 	port_t;

bool
socket_create_socket(socket_t __inout *s, proto_t p, ip_ver_t v);

void
socket_release_socket(socket_t __in *s);


bool
socket_connect(socket_t __inout *s, const char *address, port_t port);

void
socket_disconnect(socket_t __inout *s);


uint32_t
socket_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len);

void
socket_write(socket_t __in *s, uint8_t __in *buf, uint32_t len);

#endif /* LIBNET_SOCKET_H_ */
