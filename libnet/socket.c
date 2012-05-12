#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>

/* HELPER */
int
get_address_family(ip_ver_t v) {
	if(v == LIBNET_IPV4) {
		return AF_INET;
	}

	if(v == LIBNET_IPV6) {
		return AF_INET6;	
	}

	/* maybe push error here */

	return LIBNET_UNSUPPORTED;
}

int
get_type(proto_t p) {
	if(p == LIBNET_PROTOCOL_TCP) {
		return SOCK_STREAM;
	}

	if(p == LIBNET_PROTOCOL_UDP) {
		return SOCK_DGRAM;
	}

	/* maybe push error here */

	return LIBNET_UNSUPPORTED;
}

bool
resolve_hostname(ip_t __inout *ip, const char __in *hostname, port_t port, proto_t p) {
	char pb[6];
	int tmp_sock, ret;

	struct addrinfo *dest, *it, hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = get_type(p),
		.ai_flags = AI_PASSIVE
	};

	if(ip == 0 || p >= LIBNET_PROTOCOL_UNSUPPORTED) {
		/*
			libnet_error_push(LIBNET_E_INV_ARG);
		*/

		return false;
	}

	snprintf(pb, 5, "%d", port);

	if(0 != getaddrinfo(hostname, pb, &hints, &dest)) {
		/*
			check which error returned..

			libnet_error_push(LIBNET_E_XYZ)
		*/
		return false;
	}

	for(it = dest; it != NULL; it = it->ai_next) {
		tmp_sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);

		/*if(it->ai_family == AF_INET) {
			struct sockaddr_in *in = (struct sockaddr_in *)it->ai_addr;
			printf("%s\r\n", inet_ntoa(in->sin_addr));
		}*/

		if(tmp_sock <= 0) {
			continue;
		}

		ret = connect(tmp_sock, it->ai_addr, it->ai_addrlen);

#		ifdef WIN32
		closesocket(tmp_sock);
#		else if defined(NIX)
		close(tmp_sock);
#		endif

		if(ret != 0) {
			continue;
		}

		/* found */
		ret = false;

		switch(it->ai_family) {
			case AF_INET: {
				ret = true;
				struct sockaddr_in *in = (struct sockaddr_in *)it->ai_addr;
				memcpy(&ip->v4, &in->sin_addr, sizeof(struct in_addr));
			} break;

			case AF_INET6: {
				
			} break;
		}

		break;
	}

	freeaddrinfo(dest);
	return (bool)ret;
}

/* LOCAL */
void
socket_winsock_initialize(void) {
#	ifdef WIN32

	if(0 != WSACleanup()) {
		// ...
	}

#	endif
}

void
socket_winsock_cleanup(void) {
#	ifdef WIN32

	WSADATA wsaData = {0};
	if(0 != WSAStartup(MAKEWORD(2, 0), &wsaData)) {
		/*
			libnet_error_push(LIBNET_E_W32_XYZ); /* LIB _ ERROR _ [<PLATFORM>] _ <ERR>
		*/
	}

#	endif
}

bool
socket_count = 0;

/* PUBLIC */
bool
socket_create_socket(socket_t __inout *s, proto_t p, ip_ver_t v) {
	if(s == 0
	|| p >= LIBNET_PROTOCOL_UNSUPPORTED
	|| v >= LIBNET_UNSUPPORTED_VER) {
		/*
			libnet_error_push(LIBNET_E_INV_ARG);
		*/
		return false;
	}

	if(v == LIBNET_IPV6) {
		/* not supported right now */
		/*
			libnet_error_push(LIBNET_E_IPV6_NOT_SUPPORTED);
		*/
		return false;
	}

	s->handle = socket(get_address_family(v), get_type(p), 0);

	if(s->handle <= 0) {
		/*
			invalid combination of protocol and address family:
				libnet_error_push(LIBNET_E_INV_ARG);

			.. do some checks if combination is correct
		*/

		return false;
	}

	s->ip_ver = v;
	s->proto = p;

	if(++socket_count == 1) {
		socket_winsock_initialize();
	}

	/* do some checks here: socket_count < 1 */

	return true;
}

void
socket_release_socket(socket_t __in *s) {
	if(s == 0 || s->handle == 0) {
		/*
			libnet_error_push(LIBNET_E_INV_ARG);
		*/
		return;
	}

	if(--socket_count == 0) {
		socket_winsock_cleanup();
	}

	/* do some checks here: socket_count < 0 */
}

bool
socket_connect(socket_t __inout *s, const char *address, port_t port) {
	ip_t ip;

	if(s == 0 || port == 0 || address == 0) {
		/*
			libnet_error_push(LIBNET_E_INV_ARG);
		*/

		return false;
	}

	memset(&s->in, 0, sizeof(s->in)); // 28 bytes

	if(s->ip_ver == LIBNET_IPV4) {
		ip.v4 = inet_addr(address);

		if(ip.v4 == INADDR_NONE) {
			if(false == resolve_hostname(&ip, address, port, s->proto)) {
				return false;
			}
		}

		s->in.v4.sin_family = get_address_family(s->ip_ver);
		s->in.v4.sin_port = htons(port);
		memcpy(&s->in.v4.sin_addr, &ip.v4, sizeof(struct in_addr));

		if(s->proto == LIBNET_PROTOCOL_TCP) {
			if(0 != connect(s->handle, (struct sockaddr *)&s->in.v4, sizeof(struct sockaddr))) {
				/*
					libnet_eror_push(LIBNET_E_CONNECT_FAILED);
				*/

				return false;
			}
		}

		return true;
	}

	return false;
}

void
socket_disconnect(socket_t __inout *s) {
	if(s == 0) {
		/*
			libnet_eror_push(LIBNET_E_INV_ARG);
		*/

		return;
	}

#	ifdef WIN32
	closesocket(s->handle);
#	else if defined(NIX)
	close(s->handle);
#	endif

	s->handle = 0;
}

uint32_t
socket_read(socket_t __in *s, uint8_t __out *buf, uint32_t len) {
	int ret = 0;

	if(s == 0 || s->handle == 0 || buf == 0) {
		/*
			libnet_eror_push(LIBNET_E_INV_ARG);
		*/

		return;
	}

	if(s->proto == LIBNET_PROTOCOL_TCP) {
		ret = recv(s->handle, buf, len-1, 0);

		if(ret > 0) {
			buf[ret] = 0;
		}

		if(ret < 0) {
			/*
				libnet_eror_push(LIBNET_E_RECV_FAILED);
			*/
			return 0;
		}
	}

	return ret;
}

void
socket_write(socket_t __in *s, uint8_t __in *buf, uint32_t len) {
	int ret = 0;

	if(s == 0 || s->handle == 0 || buf == 0) {
		/*
			libnet_eror_push(LIBNET_E_INV_ARG);
		*/

		return;
	}

	if(s->proto == LIBNET_PROTOCOL_TCP) {
		ret = send(s->handle, buf, len, 0);

		if(ret == 0) {
			/*
				libnet_eror_push(LIBNET_E_SEND_FAILED);
			*/
		}
	}
}


