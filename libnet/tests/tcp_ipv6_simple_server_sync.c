#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>

#include <time.h>

#define HOSTNAME	"::1"
#define PORT		5501

void _sleep(long s) {
	time_t start, delta = 0;

	start = time(NULL);

	while(delta < s) {
		delta = time(NULL) - start;
	}
}

int main(void) {
	socket_t listener;
	socket_set_t clients;
	char buf[32];

	if(false == socket_create_socket(&listener, LIBNET_PROTOCOL_TCP, LIBNET_IPV6)) {
		printf("server: failed to create socket\r\n");
		return -1;
	}

	socket_create_set(&clients);

	if(false == socket_listen(&listener, PORT)) {
		printf("server: failed to listen at %d\r\n", PORT);
		return -1;
	}

	inet_ntop(listener.in.v6.sin6_family,
		&listener.in.v6.sin6_addr, buf, 31);
	printf("server: listening at %s : %d..\n", buf, PORT);

	while(socket_accept(&listener, &clients) == false) {
		_sleep(1);
	}

	printf("server: client connected (ip: %s)\r\n",
		"127.0.0.1");

	_sleep(2); // to make sure the client recognizes everything

	socket_release_set(&clients);

	socket_disconnect(&listener);
	socket_release_socket(&listener);
	
	return 0;
}
