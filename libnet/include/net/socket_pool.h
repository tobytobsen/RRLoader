/**
 * this header file contains functions to manage a pool of sockets
 *
 * @file socket_pool.h
*/
#ifndef LIBNET_SOCKET_POOL_H_
#define LIBNET_SOCKET_POOL_H_

#include <types.h>

struct socket;

/**
 * socket_pool_t holds information about the pool
*/
typedef struct socket_pool {
	uint32_t cl_cur, cl_max;
	struct socket *client;
} socket_pool_t;

/**
 * socket_create_pool() creates a pool
 *
 * @param set pool
*/
void
socket_create_pool(socket_pool_t __inout *set);

/**
 * socket_release_pool() releases a created pool
 *
 * @param set pool
*/
void
socket_release_pool(socket_pool_t __in *set);

/**
 * socket_pool_add_socket() adds a socket to a pool
 *
 * @param set pool
 * @param s socket to add
*/
void
socket_pool_add_socket(socket_pool_t __inout *set, socket_t __in *s);

/**
 * socket_pool_rem_socket() removes a socket from a pool
 *
 * @param set pool
 * @param s socket to remove
*/
void
socket_pool_rem_socket(socket_pool_t __inout *set, socket_t __in *s);

/**
 * socket_pool_get_size() is used to retrieve the amount of clients in the pool
 *
 * @param set pool
 *
 * @return returns the amount of clients in the pool
*/
uint32_t
socket_pool_get_size(socket_pool_t __in *set);

/**
 * socket_pool_get_socket() returns a specific socket in a pool
 *
 * @param set pool
 * @param i index
 *
 * @return returns the desired socket or NULL if the index is invalid
*/
socket_t*
socket_pool_get_socket(socket_pool_t __in *set, uint32_t i);

#endif /* LIBNET_SOCKET_POOL_H_ */
