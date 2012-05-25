#include <net/net.h>


static
uint32_t error = LIBNET_E_NONE;

void
libnet_error_set(uint32_t err) {
	error = err;
}

uint32_t
libnet_error_get(void) {
	return error;
}

const char*
libnet_str_error(uint32_t err) {
	switch(err) {
		default: return "unknown error";

		case LIBNET_E_NONE: 
			return "no error occurred";

		case LIBNET_E_INV_ARG: 
			return "invalid argument delivered";
		
		case LIBNET_E_IPV6_NOT_SUPPORTED: 
			return "IPv6 is not supported yet";
		case LIBNET_E_IPV6_ONLY_FAILED: 
			return "IPv6 only address lookup failed";
		
		case LIBNET_E_CONNECT_FAILED: 
			return "failed to connect to host";
		case LIBNET_E_BIND_FAILED: 
			return "failed to bind socket to desired port";
		case LIBNET_E_LISTEN_FAILED: 
			return "failed to listen";
		case LIBNET_E_RECV_FAILED: 
			return "failed to read from socket";
		case LIBNET_E_SEND_FAILED: 
			return "failed to send to destination";
		case LIBNET_E_RESOLVE_HOST: 
			return "failed to resolve hostname";

		case LIBNET_E_SET_SIZE_EXCEEDED: 
			return "socket set is full";
		case LIBNET_E_MEM:
			return "memory error. maybe not enough memory left.";
		case LIBNET_E_HASH_SIZE_EXCEEDED:
			return "hash size too large.";
		case LIBNET_E_BUFFER_FILE_BLOCKED:
			return "could not open file: file is blocked";
		case LIBNET_E_BUFFER_OFFSET_INVALID:
			return "offset is too high, using the old one.";

		case LIBNET_E_ENC_SHUTDOWN: 
			return "failed to shutdown encrypted socket";
		case LIBNET_E_ENC_NEW: 
			return "failed to create encrypted socket";
		case LIBNET_E_ENC_CONNECT: 
			return "failed to encrypt connection";
		case LIBNET_E_ENC_ACCEPT: 
			return "failed to accept client through encrypted connection";
		case LIBNET_E_ENC_SSL_CTX: 
			return "failed to create ssl context";
		case LIBNET_E_ENC_SSL_CA_CERT: 
			return "invalid ssl ca certificate";
		case LIBNET_E_ENC_SSL_CERT: 
			return "invalid ssl certificate";
		case LIBNET_E_ENC_SSL_KEY: 
			return "invalid ssl key";
	}
}
