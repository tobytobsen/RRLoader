#include <net/net.h>
#include <net/hash_table.h>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

htbl_ent_t *
find_free_entity(htbl_t *tbl, uint8_t *key, uint8_t *hash) {
	uint32_t i;

	if(tbl == NULL || (key == NULL && hash == NULL)) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}

	if(tbl->entities >= tbl->max_entities) {
		libnet_error_set(LIBNET_E_HASH_SIZE_EXCEEDED);
		return NULL;
	}

	if(key != NULL) {
		htbl_ent_t *e;
		if((e = htbl_get(tbl, key)) != NULL) {
			return e; // duplicate
		}
	}

	for(i=0; i<tbl->entities; i++) {
		if(hash != NULL && !memcmp(tbl->entity[i].hash,
			hash, LIBNET_HASH_SIZE)) {
			// duplicate
			return NULL;
		}

		if(!memcmp(tbl->entity[i].hash, 
			(char[LIBNET_HASH_SIZE]){0}, 
			LIBNET_HASH_SIZE)) {
			break;
		}
	}

	if(i == tbl->entities && tbl->entities+1 > tbl->max_entities) {
		libnet_error_set(LIBNET_E_HASH_SIZE_EXCEEDED);
		return NULL;
	} else if(i == tbl->entities && (tbl->entities+1) <= tbl->max_entities) {
		tbl->entity = realloc(tbl->entity, sizeof(htbl_ent_t) * ++tbl->entities);

		tbl->entity[i].data = NULL;
		tbl->entity[i].size = 0;

		memset(tbl->entity[i].hash, 0, LIBNET_HASH_SIZE);
	}

	return &tbl->entity[i];
}

bool
htbl_hash_gen_plain(uint8_t *hash, uint8_t *key) {
	if(hash == NULL || key == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	strncpy((char *)hash, (char *)key, LIBNET_HASH_SIZE);
	return true;
}

bool
htbl_hash_gen_md5(uint8_t *hash, uint8_t *key) {
	if(hash == NULL || key == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	memset(hash, 0, LIBNET_HASH_SIZE);

	if(NULL == MD5(key, strlen((char *)key), hash)) {
		libnet_error_set(LIBNET_E_HASH_GEN);
		return false;
	}

	return true;
}

bool
htbl_hash_gen_sha1(uint8_t *hash, uint8_t *key) {
	if(hash == NULL || key == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	memset(hash, 0, LIBNET_HASH_SIZE);

	if(NULL == SHA1(key, strlen((char *)key), hash)) {
		libnet_error_set(LIBNET_E_HASH_GEN);
		return false;
	}

	return true;
}

void
htbl_create(htbl_t *tbl, uint32_t limitation) {
	if(tbl == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	memset(tbl, 0, sizeof(htbl_t));
	tbl->max_entities = (limitation == 0 ? (uint32_t)(-1) : limitation);
	tbl->gen = LIBNET_HASH_GENERATOR_DEFAULT;
}

void
htbl_create_from_htbl(htbl_t *dest, htbl_t *src) {
	uint32_t i = 0;

	if(dest == NULL || src == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	htbl_create(dest, 0);

	dest->max_entities = src->max_entities;
	dest->entities = src->entities;

	dest->entity = calloc(1, dest->entities * sizeof(htbl_ent_t));

	if(dest->entity == NULL) {
		libnet_error_set(LIBNET_E_MEM);
		return;
	}

	for(i; i<src->entities; i++) {
		if(src->entity[i].data != NULL) {
			memcpy(&dest->entity[i], &src->entity[i], sizeof(htbl_ent_t));

			if(src->entity[i].size > 0) {
				dest->entity[i].data = calloc(1, dest->entity[i].size);

				if(dest->entity[i].data == NULL) {
					libnet_error_set(LIBNET_E_MEM);
					return;
				}

				memcpy(dest->entity[i].data, src->entity[i].data, dest->entity[i].size);
			}
		}
	}
}

void
htbl_release(htbl_t *tbl) {
	if(tbl == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(tbl->entity != NULL) {
		uint32_t i=0;

		for(i; i<tbl->entities; i++) {
			if(tbl->entity[i].size != 0) {
				free(tbl->entity[i].data);

				tbl->entity[i].size = 0;
				tbl->entity[i].data = NULL;

				memset(tbl->entity[i].hash, 0, LIBNET_HASH_SIZE);
			}
		}

		free(tbl->entity);
		tbl->entity = NULL;
	}

	tbl->entities = 0;
}

void
htbl_set_hash_generator(htbl_t *tbl, htbl_hash_gen_t gen) {
	if(tbl == NULL || gen == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	tbl->gen = gen;
}

void
htbl_insert(htbl_t *tbl, uint8_t *key, void *data) {
	htbl_ent_t *e = find_free_entity(tbl, key, NULL);

	if(e == NULL || tbl == NULL || tbl->gen == NULL) {
		return;
	}

	if(*e->hash == 0) {
		if(false == tbl->gen(e->hash, key)) {
			return;
		}
	}

	if(e->size > 0 && e->data != NULL) {
		free(e->data);
	}

	e->data = data;
	e->size = 0;
}

void
htbl_insert_copy(htbl_t *tbl, uint8_t *key, void *data, uint32_t size) {
	htbl_ent_t *e = NULL;

	if(tbl == NULL || tbl->gen == NULL || key == NULL || data == NULL || size == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	e = find_free_entity(tbl, key, NULL);

	if(e == NULL) {
		return;
	}

	if(*e->hash == 0) {
		if(false == tbl->gen(e->hash, key)) {
			return;
		}
	}

	if(e->size > 0 && e->data != NULL) {
		free(e->data);
	}

	e->data = calloc(1, size);

	if(e->data == NULL) {
		libnet_error_set(LIBNET_E_MEM);
		return;
	}

	memcpy(e->data, data, size);
	e->size = size;
}

void
htbl_copy(htbl_t *dest, htbl_t *src) {
	void *sdata;
	uint32_t si = 0, di = 0, size = 0;
	uint8_t *shash, *dhash;

	if(dest == NULL || src == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	while(NULL != htbl_enumerate(src, &si, &shash, &sdata, &size)) {
		while(NULL != htbl_enumerate(dest, &di, &dhash, NULL, NULL)) {
			if(0 == strncmp((char *)shash, (char *)dhash, LIBNET_HASH_SIZE)) {
				break;
			}
		}

		if(di == htbl_size(dest)) {
			htbl_ent_t *e = find_free_entity(dest, NULL, shash);

			if(NULL == e) {
				continue;
			}

			if(e->size > 0 && e->data != NULL) {
				free(e->data);
			}

			if(size == 0) {
				e->data = sdata;

			} else {
				e->data = calloc(1, size);
				memcpy(e->data, sdata, size);
			}

			e->size = size;
			memcpy(e->hash, shash, LIBNET_HASH_SIZE);
		}
	}
}

void
htbl_remove(htbl_t *tbl, uint8_t *key) {
	uint32_t i;

	if(tbl == NULL || key == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	for(i=0; i<tbl->entities; i++) {
		if(!memcmp(tbl->entity[i].hash, 
			(char[LIBNET_HASH_SIZE]){0}, 
			LIBNET_HASH_SIZE)) {
			break;
		}
	}

	if(i != tbl->entities) {
		memset(tbl->entity[i].hash, 0, LIBNET_HASH_SIZE);

		if(tbl->entity[i].size != 0) {
			free(tbl->entity[i].data);

			tbl->entity[i].size = 0;
			tbl->entity[i].data = NULL;

			memset(tbl->entity[i].hash, 0, LIBNET_HASH_SIZE);
		}

		tbl->entities--;
	}
}

void *
htbl_get(htbl_t *tbl, uint8_t *key) {
	uint32_t i;
	uint8_t hash[LIBNET_HASH_SIZE+1] = {0};

	if(tbl == NULL || tbl->gen == NULL|| key == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}

	if(false == tbl->gen(hash, key)) {
		// printf(":-/\n");
		return NULL;
	}

	for(i=0; i<tbl->entities; i++) {
		if(!memcmp(tbl->entity[i].hash, 
			hash, 
			LIBNET_HASH_SIZE)) {
			return tbl->entity[i].data;
		}
	}

	return NULL;
}

bool
htbl_empty(htbl_t *tbl) {
	if(tbl == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	return (tbl->entities == 0);
}

uint32_t
htbl_size(htbl_t *tbl) {
	if(tbl == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return 0;
	}

	return tbl->entities;
}

void *
htbl_enumerate(htbl_t *tbl, uint32_t *i, uint8_t **hash, void **data, uint32_t *size) {
	if(tbl == NULL || i == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}

	if(*i == tbl->entities) {
		return NULL;
	}

	if(*i > tbl->entities) {
		*i = 0;
	}

	for(*i; *i<tbl->entities; *i += 1) {

		if(!memcmp(tbl->entity[*i].hash, 
			(char[LIBNET_HASH_SIZE]){0}, 
			LIBNET_HASH_SIZE)) {
			continue;
		}

		if(hash != NULL) {
			*hash = tbl->entity[*i].hash;
		}

		if(data != NULL) {
			*data = tbl->entity[*i].data;
		}

		if(size != NULL) {
			*size = tbl->entity[*i].size;
		}

		break;
	}

	*i += 1;
	return tbl->entity[*i - 1].data;
}
