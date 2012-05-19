#include <net/mutex.h>

void
mutex_acquire(mutex_t *m) {
	while(*m == true);
	*m = true;
}

void
mutex_release(mutex_t *m) {
	*m = false;
}
