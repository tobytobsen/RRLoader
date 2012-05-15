#include <net/net.h>
#include <net/socket.h>

#include <stdio.h>
#include <string.h>

#define HOSTNAME	"localhost"
#define PORT		5500

int main(void) {
	socket_t client;

	if(false == socket_create_socket(&client, LIBNET_PROTOCOL_TCP, LIBNET_IPV4)) {
		printf("client: failed to create socket\r\n");
		return -1;
	}

	printf("client: trying to connect\r\n");

	while(false == socket_connect(&client, HOSTNAME, PORT));

	printf("client: connected\r\n");
	
	socket_disconnect(&client);
	socket_release_socket(&client);

	return 0;
}
