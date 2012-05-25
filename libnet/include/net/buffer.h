#ifndef LIBNET_BUFFER_H_
#define LIBNET_BUFFER_H_

#include <stdio.h>

#include <types.h>
#include <net/net.h>

#define LIBNET_BUFFER_CHUNK_SIZE 	4096
#define LIBNET_CACHING_SIZE 		512

#ifdef NIX
#	define LIBNET_BUFFER_FILE "/tmp/libnet_buf.tmp"
#else if defined(WIN32)
#	define LIBNET_BUFFER_FILE ".\\libnet_buf.tmp"
#endif

typedef enum buffer_mode {
	LIBNET_BM_MEMORY = 0,
	LIBNET_BM_FILE,
	LIBNET_BM_UNKNOWN,
} buffer_mode_t;

typedef struct buffer {
	char *mem;
	FILE *fd;

	uint32_t id;
	
	uint32_t chunks;
	uint32_t chunk_size;
	uint32_t offset;

	buffer_mode_t mode;

// caching not used right now
//	uint32_t caching;
} buffer_t;

/**
 * buffer_create() creates an buffer (file or memory mode)
 *
 * @param b buffer
 * @param mode buffer mode
 *
 * @return returns true if successful, otherwise false is returned
*/
bool
buffer_create(buffer_t *b, buffer_mode_t mode);

/**
 * buffer_create_from_file() creates an file buffer with given path
 *
 * @param b buffer
 * @param mode buffer mode
 *
 * @return returns true if successful, otherwise false is returned
*/
bool
buffer_create_from_file(buffer_t *b, const char *path);

/**
 * buffer_release() cleans up
 *
 * @param b buffer
*/
void
buffer_release(buffer_t *b);

/**
 * buffer_clear() cleans up the buffer, without resetting flags
 *
 * @param b buffer
 * @param c char to clean with
*/
void
buffer_clear(buffer_t *b, char c);

/**
 * buffer_set_mode() sets file or memory buffer mode
 *
 * @param b buffer
 * @param mode buffer mode
 *
 * @return returns true if successful, otherwise false is returned
*/
void
buffer_set_mode(buffer_t *b, buffer_mode_t mode);

/**
 * buffer_get() will return the memory pointer
 *
 * THIS WON'T WORK WITH FILE MODE!
 * 
 * @param b buffer
 *
 * @return (const) memory pointer
*/
const char *
buffer_get(buffer_t *b);

/**
 * buffer_write_formatted() is used like snprintf to the buffer or fprintf to the tmp file
 *
 * @param b buffer
 * @param format and args
*/
void
buffer_set_formatted(buffer_t *b, char *format, ...);

/**
 * buffer_write() writes to memory/file
 *
 * @param b buffer
 * @param n amount of times to write data to buffer
 * @param data data to write
 * @size size size of data
*/
void
buffer_write(buffer_t *b, uint32_t n, char *data, uint32_t size);

/**
 * buffer_read() reads from buffer
 *
 * @param b buffer
 * @param dest destination memory space
 * @param size size of data to be read
 *
 * @return returns the amount of data written
*/
uint32_t
buffer_read(buffer_t *b, char *dest, uint32_t size);

/**
 * buffer_seek() sets the offest pointer to offest and
 * it'll return the new offset pointer
 *
 * @param b buffer
 * @param offset to set (from the beginning)
 *
 * @return the current offset pointer
*/
uint32_t
buffer_seek(buffer_t *b, uint32_t offset);

/**
 * buffer_size() returns the size used
 *
 * @param b buffer
 *
 * @return returns the size used
*/
uint32_t
buffer_size(buffer_t *b);

/**
 * buffer_size_total() returns the size allocated until now (max size for now)
 *
 * @param b buffer
 *
 * @return returns the size allocated size
*/
uint32_t
buffer_size_total(buffer_t *b);

/**
 * buffer_size_left() returns the size left
 *
 * @param b buffer
 *
 * @return returns the size left
*/
uint32_t
buffer_size_left(buffer_t *b);

#endif /* LIBNET_BUFFER_H_ */
