#include <net/net.h>
#include <net/http.h>

#include <stdio.h>

int main(void) {
	http_response_t res;
	http_request_t req;
	http_con_t con;

	if(false == http_connect(&con, "http://www.google.de")) {
		printf("unable to connect to remote host.\r\n");
		return -1;
	}

	http_request_create(&con, &req, LIBNET_HTTP_V11, LIBNET_HTTP_GET, "/");
	http_request_exec(&con, &req, &res);
	http_request_release(&req);

	printf("Request sent. Response: %d\r\n", res.code);

	http_disconnect(&con);

	return 0;
}