#include <net/net.h>
#include <net/hash_table.h>

#include <stdio.h>

int main(void) {
	htbl_t ht;

	htbl_create(&ht, 0);
	htbl_insert(&ht, "foo", "bar", 4);
	htbl_insert(&ht, "fu", &ht, sizeof(htbl_t));

	printf("htbl[foo] = %s\n", htbl_get(htbl_get(&ht, "fu"), "foo"));

	htbl_release(&ht);

	printf("Last Error: %s (%d)\r\n", 
		libnet_str_error(libnet_error_get()), libnet_error_get());
	return 0;
}
