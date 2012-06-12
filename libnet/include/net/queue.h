#ifndef LIBNET_QUEUE_H_
#define LIBNET_QUEUE_H_

#include <types.h>

typedef struct queue_entity {
	void *data;
	uint32_t size, id;

	struct queue_entity *next, *prev;
} queue_entity_t;

typedef struct queue {
	queue_entity_t *head, *tail;
	uint32_t entites, limit;
} queue_t;

void
queue_create(queue_t *q, uint32_t limit); // limit = max entities, 0 = unlimited

void
queue_release(queue_t *q);

void
queue_push(queue_t *q, void *data);

void
queue_push_front(queue_t *q, void *data);

void
queue_push_copy(queue_t *q, void *data, uint32_t size);

void
queue_push_front_copy(queue_t *q, void *data, uint32_t size);

void *
queue_pop(queue_t *q);

void *
queue_pop_front(queue_t *q);

uint32_t
queue_size(void);

#endif /* LIBNET_QUEUE_H_ */
