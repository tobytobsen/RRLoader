#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>

#define HOSTNAME	"2a00:1450:4016:800::1014"	// google.com
#define PORT		80

int main(void) {
	socket_t s;
	char buf[4096] = {0};

	char req[] = "GET /\r\nHTTP/1.1\r\n\r\n";

	if(false == socket_create_socket(&s, LIBNET_PROTOCOL_TCP, LIBNET_IPV6)) {
		printf("failed to create socket\r\n");
		return -1;
	}

	if(false == socket_connect(&s, HOSTNAME, PORT)) {
		printf("failed to connect to %s:%d\r\n", HOSTNAME, PORT);
	}

	socket_write(&s, req, strlen(req));

	if(0 < socket_read(&s, buf, 4096)) {
		printf("read: %s", buf);
	}

	socket_disconnect(&s);
	socket_release_socket(&s);

	return 0;
}
