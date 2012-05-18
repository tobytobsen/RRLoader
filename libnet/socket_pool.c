#include <net/net.h>
#include <net/socket.h>
#include <net/socket_pool.h>

void
socket_create_pool(socket_pool_t __inout *set) {
	if(set == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return;
	}

	set->cl_cur = 0;
	set->cl_max = 0;
	set->client = 0;
}

void
socket_release_pool(socket_pool_t __in *set) {
	uint32_t i=0;

	if(set == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		
		return;
	}

	if(set->client == 0) {
		return; /* nothing to do */
	}

	while(set->cl_max > i) {
		if(set->client[i].handle != 0) {
			socket_disconnect(&set->client[i]);
			socket_release_socket(&set->client[i]);

			--set->cl_cur;
		}

		++i;
	}

	free(set->client);

	set->cl_max = 0;
	set->client = 0;
}

void
socket_pool_add_socket(socket_pool_t __inout *set, socket_t *s) {
	uint32_t i=0;

	if(set == 0 || s == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return;
	}

	if(set->cl_cur == LIBNET_SET_SIZE) {
		libnet_error_set(LIBNET_E_SET_SIZE_EXCEEDED);

		return;
	}

	if(set->client == 0) {
		set->cl_max = 1;
		set->cl_cur = 1;

		set->client = (socket_t *)calloc(1, sizeof(socket_t));

		memcpy(&set->client[0], s, sizeof(socket_t));

		return;
	}

	if(set->cl_max == set->cl_cur) {
		set->cl_max++;
		set->cl_cur++;

		set->client = realloc(set->client, set->cl_max * (sizeof(socket_t)));

		memcpy(&set->client[set->cl_max-1], s, sizeof(socket_t));

		return;
	}

	while(set->cl_max > i) {
		if(set->client[i].handle == 0) {
			memcpy(&set->client[i], s, sizeof(socket_t));
			++set->cl_cur;

			break;
		}
	}
}

void
socket_set_rem_socket(socket_pool_t __inout *set, socket_t __in *s) {
	uint32_t i=0;

	if(set == 0 || s == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return;
	}

	if(set->cl_cur == 0) {
		return; // set is empty
	}

	while(set->cl_max > i) {
		if(set->client[i].handle == s->handle) {
			socket_disconnect(&set->client[i]);
			socket_release_socket(&set->client[i]);
		}

		++i;
	}
}

uint32_t
socket_pool_get_size(socket_pool_t __in *set) {
	if(set == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return 0;
	}

	return set->cl_cur;
}

socket_t*
socket_pool_get_socket(socket_pool_t __in *set, uint32_t i) {
	if(i > LIBNET_SET_SIZE || set == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return NULL;
	}

	if(set->cl_cur < i) {
		return NULL;
	}

	if(set->client[i].handle == 0) {
		return NULL;
	}

	return &set->client[i];
}
