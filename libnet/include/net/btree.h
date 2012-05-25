#ifndef LIBNET_BTREE_H_
#define LIBNET_BTREE_H_

#include <types.h>

/* generate hash per data? */

typedef int (*btree_compare_t) (void *d1, void *d2);

typedef struct btree_node {
	void *data;

	struct btree_node *parent;
	struct btree_node *leaf_left, *leaf_right;
} btree_node_t;

typedef enum btree_order {
	BTREE_ORDER_LEFT 	= 0,
	BTREE_ORDER_RIGHT 	= 1,
} btree_order_t;

typedef struct btree {
	btree_compare_t	cmp;

	uint32_t size_total;

	btree_node_t *root;
} btree_t;

void
btree_create(btree_t *bt);

void
btree_release(btree_t *bt);

btree_node_t *
btree_node_create(void *data);

void
btree_node_release(btree_node_t *n);

void
btree_set_comparator(btree_t *bt, btree_compare_t nc);

btree_node_t *
btree_get_leaf(btree_node_t *n, int cmpr);

void
btree_insert(btree_t *bt, btree_node_t *n);

void
btree_remove(btree_t *bt, btree_node_t *n, bool release_leafs);

#endif /* LIBNET_BTREE_H_ */
