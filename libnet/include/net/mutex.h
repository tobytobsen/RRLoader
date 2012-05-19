#ifndef LIBNET_MUTEX_H_
#define LIBNET_MUTEX_H_

#include <types.h>

typedef bool mutex_t;

/**
 * mutex_acquire() tries to acquire the mutex and if it is unable to
 * it waits till it is able to
 *
 * @param m mutex
*/
void
mutex_acquire(mutex_t *m);

/**
 * mutex_release() releases the mutex
 *
 * @param m mutex
*/
void
mutex_release(mutex_t *m);

#endif /* LIBNET_MUTEX_H_ */
