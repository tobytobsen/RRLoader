#ifndef LIBNET_SOCKET_H_
#define LIBNET_SOCKET_H_

#include <types.h>

typedef int socket_t;

typedef struct socket_flags {
	uint16_t port;
	uint32_t addr;

	// ..
} socket_flags_t;

socket_t
socket_create_socket(socket_flags_t fl);

void
socket_release_socket(socket_t socket);

#endif /* LIBNET_SOCKET_H_ */
