#ifndef LIBNET_SOCKET_POOL_H_
#define LIBNET_SOCKET_POOL_H_

#include <types.h>

struct socket;

typedef struct socket_pool {
	uint32_t cl_cur, cl_max;
	struct socket *client;
} socket_pool_t;

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

#endif /* LIBNET_SOCKET_POOL_H_ */
