#include <net/net.h>
#include <net/btree.h>

#include <string.h>
#include <stdlib.h>

int btree_compare_default(void *d1, void *d2) {
	if(d1 == NULL || d2 == NULL) {
		return 0; /* maybe check if one of both isn't NULL */
	}

	if(*(char *)d1 < *(char *)d2) {
		return -1;
	} else if(*(char *)d1 > *(char *)d2) {
		return 1;
	}

	return 0;
}

void
btree_create(btree_t *bt) {
	if(bt == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	bt->cmp = btree_compare_default;
	bt->size_total = 0;
	bt->root = NULL;
}

void
btree_release(btree_t *bt) {
	btree_node_t *it, *p;

	if(bt == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(bt->root != NULL) {
		btree_remove(bt, bt->root, true);

		btree_node_release(bt->root);
		--bt->size_total;
	}
}

btree_node_t *
btree_node_create(void *data) {
	btree_node_t *n;

	if(data == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}

	n = calloc(1, sizeof(btree_node_t));
	n->data = data;

	if(n == NULL) {
		libnet_error_set(LIBNET_E_MEM);
		return NULL;
	}

	return n;
}

void
btree_node_release(btree_node_t *n) {
	if(n != NULL) {
		free(n);
	}
}

void
btree_set_comparator(btree_t *bt, btree_compare_t nc) {
	if(bt == NULL || nc == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	bt->cmp = nc;
}

btree_node_t *
btree_get_leaf(btree_node_t *n, int cmpr) {
	if(n != NULL) {
		if(cmpr >= BTREE_ORDER_RIGHT) {
			return n->leaf_right;
		} else if(cmpr <= BTREE_ORDER_LEFT) {
			return n->leaf_left;
		}
	}

	return n;
}

void
btree_insert(btree_t *bt, btree_node_t *n) {
	btree_node_t *it, *p = NULL;
	int cmpr;

	if(bt == NULL || n == NULL || n->data == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(bt->root == NULL) {
		bt->root = n;
		bt->size_total++;
		return;
	}

	it = bt->root;
	
	while(it != NULL) {
		p = it;

		cmpr = bt->cmp(n->data, it->data);
		it = btree_get_leaf(it, cmpr);
	}

	if(cmpr <= BTREE_ORDER_LEFT) {
		p->leaf_left = n;
	} else if(cmpr >= BTREE_ORDER_RIGHT) {
		p->leaf_right = n;
	}

	n->parent = p;
	bt->size_total++;
}

void
btree_remove(btree_t *bt, btree_node_t *n, bool release_leafs) {
	btree_node_t *nr, *p, **pl, *tmp;

	if(bt == NULL || n == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(n->parent != NULL) {
		p = n->parent;

		if(p->leaf_left == n) {
			pl = &p->leaf_left;
		} else {
			pl = &p->leaf_right;
		}

		*pl = NULL;
	}

	if(n->leaf_left == NULL && n->leaf_right == NULL) {
		return;
	}

	if(release_leafs == true) {
		if(n->leaf_left != NULL) {
			tmp = n->leaf_left;

			btree_remove(bt, tmp, true);
			btree_node_release(tmp);
		}

		if(n->leaf_right != NULL) {
			tmp = n->leaf_right;
			
			btree_remove(bt, tmp, true);
			btree_node_release(tmp);
		}

		return;
	}

	if(n->leaf_right != NULL) {
		nr = n->leaf_right;
		nr->parent = p;

		if(p != NULL) {
			*pl = nr;
		}

		if(n->leaf_left != NULL) {
			n->leaf_left->parent = nr;
		}

		tmp = n->leaf_right->leaf_left;
		n->leaf_right->leaf_left = NULL;

		btree_insert(bt, tmp);
	} else if(n->leaf_left != NULL) {
		n->leaf_left->parent = p;

		if(p != NULL) {
			*pl = n->leaf_left;
		}
	}
}

