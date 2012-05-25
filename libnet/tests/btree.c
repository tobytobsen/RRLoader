#include <net/net.h>
#include <net/btree.h>

int main(void) {
	btree_t bt;
	btree_node_t *n;
	char datatbl[] = {'a', 'b', 'd', 'd', 'a'};

	btree_create(&bt);
	btree_insert(&bt, btree_node_create(&datatbl[0]));
	btree_insert(&bt, btree_node_create(&datatbl[1]));
	btree_insert(&bt, btree_node_create(&datatbl[2]));
	
	n = btree_node_create(&datatbl[3]);
	btree_insert(&bt, n);
	btree_remove(&bt, n, false);
	btree_node_release(n);

	btree_insert(&bt, btree_node_create(&datatbl[4]));
	btree_release(&bt);

	printf("Last Error: %s (%d)\r\n", 
		libnet_str_error(libnet_error_get()), libnet_error_get());
	return 0;
}
