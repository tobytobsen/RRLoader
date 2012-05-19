/**
 * this file contains general stuff such as error handling
 *
 * @file net.h
*/
#ifndef LIBNET_NET_H_
#define LIBNET_NET_H_

#include <types.h>

#define LIBNET_UNSUPPORTED (unsigned int)(-1)

/**
 * libnet_error_set() sets the current errno
 *
 * @param err errno to set
*/
void
libnet_error_set(uint32_t err);

/**
 * libnet_error_get() returns the current errno
 *
 * @return returns the current errno
*/
uint32_t
libnet_error_get(void);

/**
 * libnet_str_error() is used to get a string which explains the errno
 *
 * @param err errno
 *
 * @return returns a string which explains the errno
*/
const char*
libnet_str_error(uint32_t err);

enum {
	LIBNET_E_NONE = 0,

	/* ARGS */
	LIBNET_E_INV_ARG,

	/* PROTOCOL */
	LIBNET_E_IPV6_NOT_SUPPORTED = 100,
	LIBNET_E_IPV6_ONLY_FAILED,
	
	/* LIBRARY */
	LIBNET_E_CONNECT_FAILED = 200,
	LIBNET_E_BIND_FAILED,
	LIBNET_E_LISTEN_FAILED,
	LIBNET_E_RECV_FAILED,
	LIBNET_E_SEND_FAILED,
	LIBNET_E_RESOLVE_HOST,
	
	/* INTERNAL */
	LIBNET_E_SET_SIZE_EXCEEDED = 300,

	/* SSL */
	LIBNET_E_ENC_SHUTDOWN = 400,
	LIBNET_E_ENC_NEW,
	LIBNET_E_ENC_CONNECT,
	LIBNET_E_ENC_ACCEPT,
	LIBNET_E_ENC_SSL_CTX,
	LIBNET_E_ENC_SSL_CA_CERT,
	LIBNET_E_ENC_SSL_CERT,
	LIBNET_E_ENC_SSL_KEY,

	/* PLATFORM */
	LIBNET_E_W32_STARTUP = 500,
	LIBNET_E_W32_CLEANUP,
};

#endif /* LIBNET_NET_H_ */
