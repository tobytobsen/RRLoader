#include <net/net.h>
#include <net/buffer.h>

int main(void) {
	buffer_t buf;

	buffer_create(&buf, LIBNET_BM_MEMORY);
	buffer_write_formatted(&buf, "Hallo Welt\r\nEinfach\tmal\tden\tBuffer\ttesten.\n");
	buffer_set_mode(&buf, LIBNET_BM_FILE);
	buffer_release(&buf);

	printf("\r\n\r\nLast Error: %s (%d)\n", libnet_str_error(libnet_error_get()), libnet_error_get());

	return 0;
}