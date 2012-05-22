/**
 * this header file contains basic network i/o with SSL support
 *
 * @file socket.h
*/
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

struct socket_pool;

/**
 * proto_t is used to set the socket protocol
*/
typedef enum proto {
	LIBNET_PROTOCOL_TCP = 0,
	LIBNET_PROTOCOL_UDP,
	LIBNET_PROTOCOL_UNSUPPORTED,
} proto_t;

/**
 * enc_t is used to set the encryption level
*/
typedef enum encryption {
	LIBNET_ENC_SSL_V2 = 0,
	LIBNET_ENC_SSL_V3,
	LIBNET_ENC_TLS_V1,
	LIBNET_ENC_NONE,
} enc_t;

/**
 * socket_param_t holds keys to set values with at socket_set_param() 
*/
typedef enum socket_param {
	LIBNET_P_NONE = 0,

	LIBNET_P_TIMEOUT,

	LIBNET_P_ENC_AVOID_UNTRUSTED,
} socket_param_t;

/**
 * ip_ver_t is used to set the IP version 
*/
typedef enum ip_ver {
	LIBNET_IPV4 = 0,
	LIBNET_IPV6,
	LIBNET_UNSUPPORTED_VER,
} ip_ver_t;

/**
 * socket_t is the general handle for i/o operations in this library 
*/
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

typedef union ip {
	uint32_t	v4;
	uint8_t 	_f[16];
} ip_t;

typedef uint16_t 	port_t;

#define socket_set_timeout(s, t) socket_set_param((s), LIBNET_P_TIMEOUT, ((void *)(&t)))
#define socket_can_accept(s) socket_is_readable((s))

/**
 * socket_create_socket() creates a socket handle of given protocol and IP version 
 *
 * @param s socket handle
 * @param p protocol
 * @param v IP version 
 *
 * @return returns true on success
*/
bool
socket_create_socket(socket_t __inout *s, proto_t p, ip_ver_t v);

/**
 * socket_release_socket() releases a created socket handle 
 *
 * @param s socket handle
*/
void
socket_release_socket(socket_t __in *s);

/**
 * socket_connect() connects to the desired host
 *
 * @param s socket handle
 * @param address remote/local address or hostname 
 * @param port port
 *
 * @return returns true on success 
*/
bool
socket_connect(socket_t __inout *s, const char *address, port_t port);

/**
 * socket_listen() binds the socket to a local port
 *
 * @param s socket handle
 * @param port port
 *
 * @return returns true on success
*/
bool
socket_listen(socket_t __inout *s, port_t port);

/**
 * socket_disconnect() closes a connection
 *
 * @param s socket handle
*/
void
socket_disconnect(socket_t __inout *s);

/**
 * socket_accept() accepts a client
 *
 * @param listener (bound) socket handle 
 * @param set socket pool
 *
 * @return returns true if a client connected
*/
bool
socket_accept(socket_t __in *listener, struct socket_pool __inout *pool);

/**
 * socket_async_accept() is the asynchronous version of socket_accept()
 *
 * @param listener (bound) socket handle
 * @param set socket pool
 *
 * @return returns true if a client connected
*/
bool
socket_async_accept(socket_t __in *listener, struct socket_pool __inout *pool);

/**
 * socket_read() reads from a socket
 *
 * @param s socket handle
 * @param buf data storage
 * @param len size of the storage
 *
 * @return returns bytes read
*/
uint32_t
socket_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len);

/**
 * socket_async_read() is the asynchronous version of socket_read()
 *
 * @param s socket handle
 * @param buf data storage
 * @param len size of the storage
 *
 * @return returns bytes read
*/
uint32_t
socket_async_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len);

/**
 * socket_is_readable() detects if the socket is readable
 *
 * @param s socket handle
 *
 * @return returns true if readable
*/
bool
socket_is_readable(socket_t __in *s);

/**
 * socket_write() writes to a socket
 *
 * @param s socket handle
 * @param buf data storage
 * @param len size of the storage
*/
void
socket_write(socket_t __in *s, uint8_t __in *buf, uint32_t len);

/**
 * socket_async_write() is the asynchronous version of socket_write()
 *
 * @param s socket handle
 * @param buf data storage
 * @param len size of the storage
*/
void
socket_async_write(socket_t __in *s, uint8_t __in *buf, uint32_t len);

/**
 * socket_is_writeable() detects if the socket is writeable
 *
 * @param s socket handle
 *
 * @return returns true if writeable
*/
bool
socket_is_writeable(socket_t __in *s);

/**
 * socket_set_encryption() is used to configure the socket for SSL or TLS
 *
 * @param s socket handle
 * @param enc encryption type
 * @param f_cert path to a certificate
 * @param f_key path to a key
 * @param f_ca_cert path to a CA certificate
 *
 * @return returns true on success
*/
bool
socket_set_encryption(socket_t __inout *s, enc_t enc, const char *f_cert, const char *f_key, const char *f_ca_cert);

/**
 * socket_set_param() configures the socket handle
 *
 * @param s socket handle
 * @param k configuration key
 * @param v value to set
*/
void
socket_set_param(socket_t __inout *s, socket_param_t k, void *v);

#endif /* LIBNET_SOCKET_H_ */
