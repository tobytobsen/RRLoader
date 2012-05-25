#include <net/buffer.h>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef NIX
# include <unistd.h>
#endif

#define ROUND_UP(rval, val) (((val) + ((rval) - 1)) & ~((rval) - 1))

uint32_t buffer_count = 0;

bool
request_chunks(char** buffer, uint32_t c, uint32_t cs) {
	if(buffer == NULL || c == 0) {
		return false;
	}

	*buffer = realloc(*buffer, c * cs);

	if(NULL == *buffer) {
		libnet_error_set(LIBNET_E_MEM);
		return false;
	}

	return true;
}

bool
resize_buffer(buffer_t *b, uint32_t new_size) {
	uint32_t chunks_needed = 0;

	if(b == NULL || new_size == 0) {
		return false;
	}

	chunks_needed = ROUND_UP(b->chunk_size, new_size) / b->chunk_size;

	if(false == request_chunks(&b->mem, chunks_needed, b->chunk_size)) {
		/* Arrrrr */
		return false;
	}
	
	b->chunks = chunks_needed;

	return true;
}

bool
buffer_create(buffer_t *b, buffer_mode_t mode) {
	if(b == NULL || mode >= LIBNET_BM_UNKNOWN) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	memset(b, 0, sizeof(buffer_t));

	b->id = ++buffer_count;
	b->chunk_size = LIBNET_BUFFER_CHUNK_SIZE;
	b->mode = mode;

	switch(mode) {
		case LIBNET_BM_MEMORY: {
			b->mem = calloc(++b->chunks, b->chunk_size);

			if(b->mem == NULL) {
				libnet_error_set(LIBNET_E_MEM);
				return false;
			}
		} break;

		case LIBNET_BM_FILE: {
			char tmp[512] = {0};
			snprintf(tmp, 511, "%s.%d", LIBNET_BUFFER_FILE, b->id);

			b->fd = fopen(tmp, "w");

			if(b->fd == NULL) {
				/* libnet_error_set(LIBNET_E_BUFFER_FILE_BLOCKED) */
				return false;
			}
		} break;
	}

	return true;
}

bool
buffer_create_from_file(buffer_t *b, const char *path) {
	if(b == NULL || path == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return false;
	}

	memset(b, 0, sizeof(buffer_t));

	b->id = ++buffer_count;
	b->chunk_size = LIBNET_BUFFER_CHUNK_SIZE;
	b->mode = LIBNET_BUFFER_FILE;

	b->fd = fopen(path, "w");

	if(b->fd == NULL) {
		libnet_error_set(LIBNET_E_BUFFER_FILE_BLOCKED);
		return false;
	}

	return true;
}

void
buffer_release(buffer_t *b) {
	if(b == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	switch(b->mode) {
		case LIBNET_BM_MEMORY: {
			free(b->mem);

			b->mem = NULL;
			b->chunks = 0;
		} break;

		case LIBNET_BM_FILE: {
			fclose(b->fd);

			b->fd = NULL;

			char tmp[512] = {0};
			snprintf(tmp, 511, "%s.%d", LIBNET_BUFFER_FILE, b->id);

#			ifdef NIX
			unlink(tmp);
#			else if defined(WIN32)
			DeleteFile(tmp);
#			endif
		} break;
	}
}

void
buffer_set_mode(buffer_t *b, buffer_mode_t mode) {
	if(b->mode == mode) {
		return;
	}

	if(b == NULL || mode >= LIBNET_BM_UNKNOWN) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	switch(mode) {
		case LIBNET_BM_FILE: {
			char tmp[512] = {0};
			snprintf(tmp, 511, "%s.%d", LIBNET_BUFFER_FILE, b->id);

			b->fd = fopen(tmp, "w");

			if(b->fd == NULL) {
				libnet_error_set(LIBNET_E_BUFFER_FILE_BLOCKED)
				return;
			}

			if(buffer_size(b) > 0) {
				fwrite(b->mem, buffer_size(b), 1, b->fd);

				free(b->mem);
				
				b->chunks = 0;
				b->mem = NULL;
			}
		} break;

		case LIBNET_BM_MEMORY: {
			if(b->mem != NULL) {
				free(b->mem);
			}

			if(false == resize_buffer(b, buffer_size_total(b))) {
				return;
			}

			fread(b->mem, buffer_size_total(b), 1, b->fd);
			fclose(b->fd);

			b->fd = NULL;
		} break;
	}

	b->mode = mode;
}

const char *
buffer_get(buffer_t *b) {
	if(b == NULL || b->mode == LIBNET_BM_FILE) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return NULL;
	}

	return (const char *)(b->mem);
}

void
buffer_write_formatted(buffer_t *b, char *format, ...) {
	va_list vl;
	uint32_t len = 0, tmp = 0;
	int ret = 0;

	if(b == NULL || format == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	va_start(vl, format);
	len = buffer_size_left(b);

	switch(b->mode) {
		case LIBNET_BM_FILE: {
			ret = vfprintf(b->fd, format, vl);

			if(ret >= 0) {
				b->offset = fseek(b->fd, 0, SEEK_END);
			}
		} break;

		case LIBNET_BM_MEMORY: {
			do {
				ret = vsnprintf(b->mem + b->offset, len - 1, format, vl);

				if(ret > 0 && ret >= len) {
					if(false == resize_buffer(b, buffer_size_total(b) + ret)) {
						/* Arrrrr */
						va_end(vl);

						return;
					}

					len = buffer_size_left(b);
				}
			} while(ret > 0 && len < ret);

			b->offset += ret;
		} break;
	}

	va_end(vl);
}

void
buffer_write(buffer_t *b, char *data, uint32_t size) {
	if(b == NULL || data == NULL || size == 0) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return;
	}

	switch(b->mode) {
		case LIBNET_BM_FILE: {
			fwrite(data, size, 1, b->fd);

			b->offset = fseek(b->fd, 0, SEEK_END);
		} break;

		case LIBNET_BM_MEMORY: {
			if(buffer_size_left(b) < size) {
				if(false == resize_buffer(b, buffer_size_total(b) + size)) {
					return;
				}
			}

			memcpy(b->mem + b->offset, data, size);
		} break;
	}
}

uint32_t
buffer_read(buffer_t *b, char *dest, uint32_t size) {
	int len = 0;

	if(b == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return 0;
	}

	switch(b->mode) {
		case LIBNET_BM_FILE: {
			len = fread(dest, size, 1, b->fd);
			b->offset = fseek(b->fd, 0, SEEK_END);
		} break;

		case LIBNET_BM_MEMORY: {
			len = buffer_size_left(b) - size;

			if(len > 0) {
				len = 0;
			} else {
				len -= size;
			}

			memcpy(dest, b->mem + b->offset, len);
		} break;
	}

	return len;
}

uint32_t
buffer_seek(buffer_t *b, uint32_t offset) {
	if(b == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return 0;
	}

	switch(b->mode) {
		case LIBNET_BM_FILE: {
			if(0 != fseek(b->fd, offset, SEEK_SET)) {
				return b->offset;
			} else {
				libnet_error_set(LIBNET_E_BUFFER_OFFSET_INVALID);
				return b->offset;
			}
		} break;

		case LIBNET_BM_MEMORY: {
			if(buffer_size_total(b) < offset) {
				libnet_error_set(LIBNET_E_BUFFER_OFFSET_INVALID);
				return b->offset;
			}		
		} break;
	}

	return (b->offset = offset);
}

uint32_t
buffer_size(buffer_t *b) {
	if(b == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return 0;
	}

	return b->offset;
}

uint32_t
buffer_size_total(buffer_t *b) {
	if(b == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return 0;
	}

	uint32_t st = 0;

	switch(b->mode) {
		case LIBNET_BM_FILE: {
			fseek(b->fd, 0, SEEK_END);
			st = ftell(b->fd);
			fseek(b->fd, b->offset, SEEK_SET);
		} break;

		case LIBNET_BM_MEMORY: {
			st = (b->chunks * b->chunk_size);
		} break;
	}

	return st;
}

uint32_t
buffer_size_left(buffer_t *b) {
	if(b == NULL) {
		libnet_error_set(LIBNET_E_INV_ARG);
		return 0;
	}

	return buffer_size_total(b) - buffer_size(b);
}
