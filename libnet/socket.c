#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint32_t
socket_count = 0;

bool
enc_used = false;

/* HELPER */
int
get_address_family(ip_ver_t v) {
	if(v == LIBNET_IPV4) {
		return AF_INET;
	}

	if(v == LIBNET_IPV6) {
		return AF_INET6;	
	}

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

	return LIBNET_UNSUPPORTED;
}

bool
find_suitable_info(socket_t __inout *s, const char __in *hostname, port_t port, bool listener) {
	char pb[6];
	int tmp_sock, ret;

	struct addrinfo *dest, *it, hints = {
		.ai_family = get_address_family(s->ip_ver),
		.ai_socktype = get_type(s->proto),
		.ai_flags = AI_PASSIVE
	};

	if(s == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return false;
	}

	snprintf(pb, 5, "%d", port);

	if(0 != getaddrinfo(hostname, pb, &hints, &dest)) {
		libnet_error_set(LIBNET_E_RESOLVE_HOST);

		return false;
	}

	for(it = dest; it != NULL; it = it->ai_next) {
		tmp_sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);

		if(tmp_sock <= 0) {
			continue;
		}

		if(listener == true) {
			ret = bind(tmp_sock, it->ai_addr, it->ai_addrlen);
		} else {
			ret = connect(tmp_sock, it->ai_addr, it->ai_addrlen);
		}

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
				memcpy(&s->in.v4, it->ai_addr, it->ai_addrlen);
			} break;

			case AF_INET6: {
				ret = true;
				memcpy(&s->in.v6, it->ai_addr, it->ai_addrlen);
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
		libnet_error_set(LIBNET_E_W32_CLEANUP);
	}

#	endif
}

void
socket_winsock_cleanup(void) {
#	ifdef WIN32

	WSADATA wsaData = {0};
	if(0 != WSAStartup(MAKEWORD(2, 0), &wsaData)) {
		libnet_error_set(LIBNET_E_W32_STARTUP);
	}

#	endif
}

/* PUBLIC */
bool
socket_create_socket(socket_t __inout *s, proto_t p, ip_ver_t v) {
	if(s == 0
	|| p >= LIBNET_PROTOCOL_UNSUPPORTED
	|| v >= LIBNET_UNSUPPORTED_VER) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	memset(s, 0, sizeof(socket_t));

	s->handle = socket(get_address_family(v), get_type(p), 0);

	if(s->handle <= 0) {
		/*
			invalid combination of protocol and address family:
				libnet_error_set(LIBNET_E_INV_ARG);

			.. do some checks if combination is correct
		*/

		return false;
	}

	s->ip_ver = v;
	s->proto = p;
	s->enc = LIBNET_ENC_NONE;

	if(++socket_count == 1) {
		socket_winsock_initialize();
	}

	/* do some checks here: socket_count < 1 */

	return true;
}

void
socket_release_socket(socket_t __in *s) {
	if(s == 0 || s->handle == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(--socket_count == 0 && s->is_client == false) {
		socket_winsock_cleanup();
	}

	if(s->enc != LIBNET_ENC_NONE) {
		switch(s->enc) {
			case LIBNET_ENC_SSL_V2:
			case LIBNET_ENC_SSL_V3: {
				SSL_free(s->ssl.handle);

				if(s->is_client == false) {
					SSL_CTX_free(s->ssl.ctx);
				}
			} break;

			case LIBNET_ENC_TLS_V1: {

			} break;
		}
	}

	/* do some checks here: socket_count < 0 */
}

bool
socket_connect(socket_t __inout *s, const char *address, port_t port) {
	ip_t ip;
	long r;
	struct sockaddr *info;
	uint32_t infolen;
	char *t = NULL;

	if(s == 0 || port == 0 || address == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return false;
	}

	memset(&s->in, 0, sizeof(s->in)); // 28 bytes

	if(s->ip_ver == LIBNET_IPV4) {
		ip.v4 = inet_addr(address);

		if(ip.v4 == INADDR_NONE) {
			if(false == find_suitable_info(s, address, port, false)) {
				return false;
			}
		} else {
			s->in.v4.sin_family = get_address_family(s->ip_ver);
			s->in.v4.sin_port = htons(port);
			memcpy(&s->in.v4.sin_addr, &ip.v4, sizeof(struct in_addr));
		}

		info = (struct sockaddr *)(&s->in.v4);
		infolen = sizeof(s->in.v4);
	} else if(s->ip_ver == LIBNET_IPV6) {
		info = (struct sockaddr *)(&s->in.v6);
		infolen = sizeof(s->in.v6);

#		ifdef WIN32
		r = RtlIpv6StringToAddress(address, t, &s->in.v6);

		if(r != NO_ERROR) {
#		else if defined NIX
		r = inet_pton(get_type(s->proto), address, &s->in.v6);

		if(r != 1) {
#		endif
			if(false == find_suitable_info(s, address, port, false)) {
				return false;
			}	
		}
	}

	if(s->proto == LIBNET_PROTOCOL_TCP) {
		int ret = connect(s->handle, info, infolen);

		if(0 != ret) {
			libnet_error_set(LIBNET_E_CONNECT_FAILED);
			return false;
		}

		if(s->enc != LIBNET_ENC_NONE) {
			ret = 1;

			switch(s->enc) {
				case LIBNET_ENC_SSL_V2:
				case LIBNET_ENC_SSL_V3: {
					ret = SSL_connect(s->ssl.handle);
				} break;

				case LIBNET_ENC_TLS_V1: {

				} break;
			}

			if(ret != 1) {
				libnet_error_set(LIBNET_E_ENC_CONNECT);

				return false;
			}
		}

		return true;
	}

	return false;
}

bool
socket_listen(socket_t __inout *s, port_t port) {
	ip_t ip;
	uint32_t w;
	struct sockaddr *info;
	uint32_t infolen;

	if(s == 0 || port == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return false;
	}

	memset(&s->in, 0, sizeof(s->in)); // 28 bytes

	if(s->ip_ver == LIBNET_IPV4) {
		w = INADDR_ANY;

		s->in.v4.sin_family = get_address_family(s->ip_ver);
		s->in.v4.sin_port = htons(port);
		memcpy(&s->in.v4.sin_addr, &w, sizeof(struct in_addr));

		info = (struct sockaddr *)(&s->in.v4);
		infolen = sizeof(s->in.v4);
	} else if(s->ip_ver == LIBNET_IPV6) {
		if(false == find_suitable_info(s, 0, port, true)) {
			return false;
		}

		info = (struct sockaddr *)(&s->in.v6);	
		infolen = sizeof(s->in.v6);

		int on = 1;
		w = setsockopt(s->handle, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));

		if(w != 0) {
			libnet_error_set(LIBNET_E_IPV6_ONLY_FAILED);
			return false;
		}
	}

	if(s->proto == LIBNET_PROTOCOL_TCP) {
		if(0 != bind(s->handle, info, infolen)) {
			libnet_error_set(LIBNET_E_BIND_FAILED);
			return false;
		}

		if(0 != listen(s->handle, 0)) {
			libnet_error_set(LIBNET_E_LISTEN_FAILED);
			return false;
		}

		return true;
	}

	return false;
}

void
socket_disconnect(socket_t __inout *s) {
	if(s == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return;
	}

	if(s->enc != LIBNET_ENC_NONE) {
		int ret = 1;

		switch(s->enc) {
			case LIBNET_ENC_TLS_V1:
			case LIBNET_ENC_SSL_V2:
			case LIBNET_ENC_SSL_V3: {
				if(s->ssl.handle != NULL) {
					ret = SSL_shutdown(s->ssl.handle);
				}
			} break;
		}

		if(ret != 1) {
			libnet_error_set(LIBNET_E_ENC_SHUTDOWN);
			
			return;
		}
	}

#	ifdef WIN32
	closesocket(s->handle);
#	else if defined(NIX)
	close(s->handle);
#	endif

	s->handle = 0;
}

bool
socket_accept(socket_t __in *listener, socket_set_t __inout *set) {
	int ret = 0;

	if(listener == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return 0;
	}

	if(listener->proto == LIBNET_PROTOCOL_TCP) {
		socket_t cl = {0};

		struct sockaddr info;
		uint32_t info_len = sizeof(struct sockaddr);

		ret = accept(listener->handle, &info, &info_len);

		if(ret <= 0) {
			return false;
		}

		memcpy(&cl, listener, sizeof(socket_t));
		cl.is_client = true;

		if(listener->enc != LIBNET_ENC_NONE) {
			switch(listener->enc) {
				case LIBNET_ENC_TLS_V1:
				case LIBNET_ENC_SSL_V2:
				case LIBNET_ENC_SSL_V3: {
					cl.ssl.handle = SSL_new(listener->ssl.ctx);

					if(cl.ssl.handle == NULL) {
						libnet_error_set(LIBNET_E_ENC_NEW);

						return false;
					}

					SSL_set_fd(cl.ssl.handle, cl.handle);

					if(1 != SSL_accept(cl.ssl.handle)) {
						libnet_error_set(LIBNET_E_ENC_ACCEPT);

						return false;
					}
				} break;
			}
		}

		if(listener->ip_ver == LIBNET_IPV4) {
			memcpy(&cl.in.v4, &info, sizeof(struct sockaddr_in));
		} else if (listener->ip_ver == LIBNET_IPV6) {
			memcpy(&cl.in.v6, &info, sizeof(struct sockaddr_in6));
		}
		
		cl.handle = ret;

		/* where to store? */
		socket_set_add_socket(set, &cl);
	}

	return ret;
}

bool
socket_async_accept(socket_t __in *listener, socket_set_t __inout *set) {
	fd_set rs;

	if(listener == 0 || set == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return false;
	}

	FD_ZERO(&rs);
	FD_SET(listener->handle, &rs);

	if(select(listener->handle + 1, &rs, NULL, NULL, &listener->timeout) > 0) {
		return socket_accept(listener, set);
	}

	return 0;
}

uint32_t
socket_read(socket_t __in *s, uint8_t __out *buf, uint32_t len) {
	int ret = 0;
	struct sockaddr* info;
	uint32_t infolen;

	if(s == 0 || s->handle == 0 || buf == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return 0;
	}

	if(s->ip_ver == LIBNET_IPV4) {
		info = (struct sockaddr *)(&s->in.v4);
		infolen = sizeof(s->in.v4);
	} else if (s->ip_ver == LIBNET_IPV6) {
		info = (struct sockaddr *)(&s->in.v6);
		infolen = sizeof(s->in.v6);
	}

	if(s->proto == LIBNET_PROTOCOL_TCP) {
		switch(s->enc) {
			case LIBNET_ENC_TLS_V1:
			case LIBNET_ENC_SSL_V2:
			case LIBNET_ENC_SSL_V3: {
				ret = SSL_read(s->ssl.handle, buf, len);
			} break;

			case LIBNET_ENC_NONE: {
				ret = recv(s->handle, buf, len-1, 0);
			} break;
		}
	} else if(s->proto == LIBNET_PROTOCOL_UDP) {
		ret = recvfrom(s->handle, buf, len, 0, info, &infolen);
	}

	if(ret > 0) {
		buf[ret] = 0;
	}

	if(ret < 0) {
		libnet_error_set(LIBNET_E_RECV_FAILED);
		return 0;
	}


	return ret;
}

uint32_t
socket_async_read(socket_t __in *s, uint8_t __inout *buf, uint32_t len) {
	fd_set rs;

	if(s == 0 || s->handle == 0 || buf == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return 0;
	}

	FD_ZERO(&rs);
	FD_SET(s->handle, &rs);

	if(select(s->handle + 1, &rs, NULL, NULL, &s->timeout) > 0) {
		return socket_read(s, buf, len);
	}

	return 0;
}

void
socket_write(socket_t __in *s, uint8_t __in *buf, uint32_t len) {
	int ret = 0;
	struct sockaddr *info;
	uint32_t infolen;

	if(s == 0 || s->handle == 0 || buf == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return;
	}

	if(s->ip_ver == LIBNET_IPV4) {
		info = (struct sockaddr *)(&s->in.v4);
		infolen = sizeof(s->in.v4);
	} else if (s->ip_ver == LIBNET_IPV6) {
		info = (struct sockaddr *)(&s->in.v6);
		infolen = sizeof(s->in.v6);
	}

	if(s->proto == LIBNET_PROTOCOL_TCP) {
		switch(s->enc) {
			case LIBNET_ENC_TLS_V1:
			case LIBNET_ENC_SSL_V2:
			case LIBNET_ENC_SSL_V3: {
				ret = SSL_write(s->ssl.handle, buf, len);
			} break;

			case LIBNET_ENC_NONE: {
				ret = send(s->handle, buf, len, 0);
			} break;
		}
	} else if(s->proto == LIBNET_PROTOCOL_UDP) {
		ret = sendto(s->handle, buf, len, 0, info, infolen);
	}

	if(ret == 0) {
		libnet_error_set(LIBNET_E_SEND_FAILED);
	}
}

void
socket_async_write(socket_t __in *s, uint8_t __in *buf, uint32_t len) {
	fd_set ws;

	if(s == 0 || s->handle == 0 || buf == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return;
	}

	FD_ZERO(&ws);
	FD_SET(s->handle, &ws);

	if(select(s->handle + 1, NULL, &ws, NULL, &s->timeout) > 0) {
		socket_write(s, buf, len);
	}
}

void
socket_set_timeout(socket_t __inout *s, struct timeval t) {
	memcpy(&s->timeout, &t, sizeof(struct timeval));
}

bool
socket_set_encryption(socket_t __inout *s, enc_t enc, const char* f_cert, const char *f_key) {
	int ret;

	if(s == NULL || f_key == NULL || f_cert == NULL || enc == LIBNET_ENC_NONE) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	

	switch(enc) {
		case LIBNET_ENC_TLS_V1: {
			s->ssl.method = TLSv1_method();
		} break;
		
		case LIBNET_ENC_SSL_V2: {
			s->ssl.method = SSLv2_method();
		} break;
		
		case LIBNET_ENC_SSL_V3: {
			s->ssl.method = SSLv3_method();
		} break;

		default: {
			libnet_error_set(LIBNET_E_INV_ARG);
			return false;
		} break;
	}

	if(enc_used == false) {
		SSL_library_init();
		SSL_load_error_strings();

		enc_used = true;
	}

	s->enc = enc;
	s->ssl.ctx = SSL_CTX_new(s->ssl.method);

	if(s->ssl.ctx == NULL) {
		libnet_error_set(LIBNET_E_ENC_SSL_CTX);

		return false;
	}

	ret = SSL_CTX_use_certificate_file(s->ssl.ctx, f_cert, SSL_FILETYPE_PEM);

	if(ret <= 0) {
		libnet_error_set(LIBNET_E_ENC_SSL_CERT);

		return false;
	}

	ret = SSL_CTX_use_PrivateKey_file(s->ssl.ctx, f_key, SSL_FILETYPE_PEM);

	if(ret <= 0) {
		libnet_error_set(LIBNET_E_ENC_SSL_KEY);

		return false;
	}

	if(0 == SSL_CTX_check_private_key(s->ssl.ctx)) {
		libnet_error_set(LIBNET_E_ENC_SSL_CERT);

		return false;
	}

	SSL_CTX_set_verify(s->ssl.ctx, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify_depth(s->ssl.ctx, 1);

	s->ssl.handle = SSL_new(s->ssl.ctx);

	if(s->ssl.handle == NULL) {
		libnet_error_set(LIBNET_E_ENC_NEW);

		return false;
	}

	SSL_set_fd(s->ssl.handle, s->handle);
	return true;
}

void
socket_create_set(socket_set_t __inout *set) {
	if(set == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return;
	}

	set->cl_cur = 0;
	set->cl_max = 0;
	set->client = 0;
}

void
socket_release_set(socket_set_t __in *set) {
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
socket_set_add_socket(socket_set_t __inout *set, socket_t *s) {
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
socket_set_rem_socket(socket_set_t __inout *set, socket_t __in *s) {
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
socket_set_get_client_amount(socket_set_t __in *set) {
	if(set == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);

		return 0;
	}

	return set->cl_cur;
}

socket_t*
socket_set_get_client(socket_set_t __in *set, uint32_t i) {
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
