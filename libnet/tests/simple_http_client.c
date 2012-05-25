#include <net/net.h>
#include <net/http.h>

#include <stdio.h>

void http_read(uint32_t rid, char *data, uint32_t len) {
	printf("Request #%d: received %d bytes\n", rid, len);
	//printf("%s", data);
}

int main(void) {
	http_response_t res;
	http_request_t req;
	http_con_t con;

	if(false == http_connect(&con, "http://www.google.de", LIBNET_HTTP_V11)) {
		printf("unable to connect to remote host.\r\n");
		return -1;
	}

	http_request_create(&con, &req, LIBNET_HTTP_GET, "/");
	http_request_set_callback(&req, LIBNET_HTTP_CBT_READ, (void *)http_read);
	http_request_exec(&con, &req, &res);
	http_request_release(&req);

	http_disconnect(&con);

	printf("\r\nLast Error: %s (%d)\n", libnet_str_error(libnet_error_get()), libnet_error_get());

	return 0;
}