#include <net/net.h>
#include <net/hash_table.h>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
htbl_release(htbl_t *tbl) {
	if(tbl == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	if(tbl->entity != NULL) {
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

	tbl->entity[i].data = data;
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
	}
}

void *
htbl_get(htbl_t *tbl, uint8_t *key) {
	uint32_t i;
	uint8_t hash[LIBNET_HASH_SIZE] = {0};

	if(false == tbl->gen(hash, key)) {
		return NULL;
	}

	if(tbl == NULL || key == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
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
