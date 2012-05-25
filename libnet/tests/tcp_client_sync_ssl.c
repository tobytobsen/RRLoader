#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>

#define HOSTNAME	"www.google.de"
#define PORT		443

#define SSL_CERT	"tests/ssl/ca.crt"
#define SSL_KEY		"tests/ssl/ca.key"

int main(void) {
	socket_t s;
	uint8_t buf[4096] = {0};

	uint8_t req[] = "GET /\r\nHTTP/1.1\r\n\r\n";

	if(false == socket_create_socket(&s, LIBNET_PROTOCOL_TCP, LIBNET_IPV4)) {
		printf("failed to create socket\r\n");
		return -1;
	}

	if(false == socket_set_encryption(&s, LIBNET_ENC_SSL_V3, SSL_CERT, SSL_KEY, NULL)) {
		printf("unable to use SSLv3 encryption\r\n");
	}

	if(false == socket_connect(&s, HOSTNAME, PORT)) {
		printf("failed to connect to %s:%d\r\n", HOSTNAME, PORT);
	} else {
		printf("connected to %s : %d\r\nSending HTTP Request..\r\n", HOSTNAME, PORT);
	}

	socket_write(&s, req, strlen((char*)req));

	if(0 < socket_read(&s, buf, 4096)) {
		printf("read: %s", buf);
	}

	socket_disconnect(&s);
	socket_release_socket(&s);

	printf("Latest error: %d (%s)\n", libnet_error_get(), libnet_str_error(libnet_error_get()));

	return 0;
}
