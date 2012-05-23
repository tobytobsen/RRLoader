#ifndef LIBNET_HASHTBL_H_
#define LIBNET_HASHTBL_H_

#include <types.h>

/*
 todo: allocation? ..
*/

#define LIBNET_HASH_SIZE 				32
#define LIBNET_HASH_GENERATOR_DEFAULT 	htbl_hash_gen_md5

typedef bool (*htbl_hash_gen_t)(uint8_t *hash, uint8_t *key);

typedef struct htbl_ent {
	uint8_t hash[LIBNET_HASH_SIZE];
	void *data;
	uint32_t size;
	//bool allocated;
} htbl_ent_t;

typedef struct htbl {
	uint32_t entities;
	uint32_t max_entities;

	htbl_ent_t *entity;

	htbl_hash_gen_t gen;
} htbl_t;

bool
htbl_hash_gen_plain(uint8_t *hash, uint8_t *key);

bool
htbl_hash_gen_md5(uint8_t *hash, uint8_t *key); // default

bool
htbl_hash_gen_sha1(uint8_t *hash, uint8_t *key);

void
htbl_create(htbl_t *tbl, uint32_t limitation);

void
htbl_release(htbl_t *tbl);

void
htbl_set_hash_generator(htbl_t *tbl, htbl_hash_gen_t gen);

void
htbl_insert(htbl_t *tbl, uint8_t *key, void *data, uint32_t size);

void
htbl_remove(htbl_t *tbl, uint8_t *key);

void *
htbl_get(htbl_t *tbl, uint8_t *key);

bool
htbl_empty(htbl_t *tbl);

uint32_t
htbl_size(htbl_t *tbl);

void *
htbl_enumerate(htbl_t *tbl, uint32_t *i, uint8_t **hash, void **data);

#endif /* LIBNET_HASHTBL_H_ */
