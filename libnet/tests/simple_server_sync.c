#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>

#include <time.h>

#define HOSTNAME	"localhost"
#define PORT		5500

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

	if(false == socket_create_socket(&listener, LIBNET_PROTOCOL_TCP, LIBNET_IPV4)) {
		printf("server: failed to create socket\r\n");
		return -1;
	}

	socket_create_set(&clients);

	if(false == socket_listen(&listener, PORT)) {
		printf("server: failed to listen at %d\r\n", PORT);
		return -1;
	}

	printf("server: listening at %d..\n", PORT);

	while(socket_set_get_client_amount(&clients) < 1) {
		socket_accept(&listener, &clients);
	}

	printf("server: client connected (ip: %s)\r\n",
		"127.0.0.1");

	_sleep(2); // to make sure the client recognizes everything

	socket_release_set(&clients);

	socket_disconnect(&listener);
	socket_release_socket(&listener);
	
	return 0;
}
