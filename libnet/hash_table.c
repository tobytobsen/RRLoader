#include <net/net.h>
#include <net/hash_table.h>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	// printf("ASDASD\n");
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
htbl_release(htbl_t *tbl) {
	if(tbl == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(tbl->entity != NULL) {
		uint32_t i=0;

		for(i; i<tbl->entities; i++) {
			if(tbl->entity[i].data != NULL) {
				free(tbl->entity[i].data);
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
htbl_insert(htbl_t *tbl, uint8_t *key, void *data, uint32_t size) {
	uint32_t i;

	if(tbl == NULL || key == NULL/* || data == NULL*/) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(tbl->entities >= tbl->max_entities) {
		libnet_error_set(LIBNET_E_HASH_SIZE_EXCEEDED);
		return;
	}

	if(htbl_get(tbl, key) != NULL) {
		return; // duplicate
	}

	for(i=0; i<tbl->entities; i++) {
		if(!memcmp(tbl->entity[i].hash, 
			(char[LIBNET_HASH_SIZE]){0}, 
			LIBNET_HASH_SIZE)) {
			break;
		}
	}

	if(i == tbl->entities && tbl->entities+1 > tbl->max_entities) {
		libnet_error_set(LIBNET_E_HASH_SIZE_EXCEEDED);
		return;
	} else if(i == tbl->entities && tbl->entities+1 <= tbl->max_entities) {
		tbl->entity = realloc(tbl->entity, sizeof(htbl_ent_t) * ++tbl->entities);
	}
	
	if(false == tbl->gen(tbl->entity[i].hash, key)) {
		return;
	}

	tbl->entity[i].data = calloc(1, size);

	if(tbl->entity[i].data == NULL) {
		libnet_error_set(LIBNET_E_MEM);
		return;
	}

	memcpy(tbl->entity[i].data, data, size);
	tbl->entity[i].size = size;
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

		if(tbl->entity[i].data != NULL) {
			free(tbl->entity[i].data);
			tbl->entity[i].data = NULL;
		}

		tbl->entities--;
	}
}

void *
htbl_get(htbl_t *tbl, uint8_t *key) {
	uint32_t i;
	uint8_t hash[LIBNET_HASH_SIZE] = {0};

	// printf("key: %s\n", key);

	if(false == tbl->gen(hash, key)) {
		// printf(":-/\n");
		return NULL;
	}

	// printf("key: %s\n", key);

	if(tbl == NULL || key == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
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
htbl_enumerate(htbl_t *tbl, uint32_t *i, uint8_t **hash, void **data) {
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

		*i += 1;

		return tbl->entity[*i - 1].data;
	}
}
