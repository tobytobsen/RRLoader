#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>

#define HOSTNAME	"www.google.de"
#define PORT		80

int main(void) {
	socket_t s;
	char buf[4096] = {0};
	struct timeval t = {1, 0};

	char req[] = "GET /\r\nHTTP/1.1\r\n\r\n";

	if(false == socket_create_socket(&s, LIBNET_PROTOCOL_TCP, LIBNET_IPV4)) {
		printf("failed to create socket\r\n");
		return -1;
	}

	if(false == socket_connect(&s, HOSTNAME, PORT)) {
		printf("failed to connect to %s:%d\r\n", HOSTNAME, PORT);
	}

	socket_set_timeout(&s, t);

	socket_async_write(&s, req, strlen(req));

	if(0 < socket_async_read(&s, buf, 4096)) {
		printf("read: %s", buf);
	}

	socket_disconnect(&s);
	socket_release_socket(&s);

	return 0;
}
