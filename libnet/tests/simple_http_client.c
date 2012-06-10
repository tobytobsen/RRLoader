#include <net/net.h>
#include <net/http.h>
#include <net/http_header.h>
#include <net/http_options.h>

#include <stdio.h>

int main(void) {
	http_ctx_t ctx;

	if(false == http_connect(&ctx, "http://board.raidrush.ws/forumdisplay.php")) {
		printf("unable to connect to remote host.\r\n");
		return -1;
	}

	http_option_set(&ctx, LIBNET_HTTP_OPT_PARAM, "f=13");
	http_execute(&ctx);
	http_disconnect(&ctx);

	printf("\r\nLast Error: %s (%d)\n", libnet_str_error(libnet_error_get()), libnet_error_get());

	return 0;
}