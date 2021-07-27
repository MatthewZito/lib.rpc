/**
 * @file serialize.c
 * @author Matthew Zito (goldmund@freenode)
 * @brief Implements a serialization format, generally in the interest of RPC support
 * @version 0.1
 * @date 2021-07-26
 *
 * @copyright Copyright (c) 2021 Matthew Zito (goldmund)
 *
 */
#include "librpc.h"

#include <stdlib.h>
#include <memory.h>

/* initializers */

/**
 * @brief Initialize a serialized buffer;
 * if provided capacity <= 0, defaults to lib default
 *
 * @param sb
 * @param capacity
 */
void rpc_serialbuf_init(serialbuf_t** sb, int capacity) {
	if (capacity <= 0) capacity = RPC_SERIALBUF_SIZE;

	(*sb) = (serialbuf_t*)malloc(sizeof(serialbuf_t));
	(*sb)->buffer = malloc(capacity);
	(*sb)->capacity = capacity;
	(*sb)->cursor = 0;
}

/* serializers */

/**
 * @brief Serialize the provided data into the given buffer
 *
 * @param sb
 * @param data
 * @param size
 * @return int 1 success, 0 fail
 */
int rpc_serialize_data(serialbuf_t* sb, char* data, int size) {
	if (!sb) return 0;

	serialbuf_t* buffer = (serialbuf_t*)(sb);
	int available_size = buffer->capacity - buffer->cursor;
	int resized;

	// double the buffer size until available >= size
	while (available_size < size) {
		buffer->capacity = buffer->capacity * 2;
		available_size = buffer->capacity - buffer->cursor;
		resized = 1;
	}

	// copy data into buffer at cursor pos
	if (resized) {
		// resize buffer
	  if (!(buffer->buffer = realloc(buffer->buffer, buffer->capacity))) {
			return 0;
		}
	}

	// TODO use alternative
	memcpy((char*)buffer->buffer + buffer->cursor, data, size);

	buffer->cursor += size;

	return 1;
}

/**
 * @brief Deserialize the buffer's data into `dest`
 *
 * @param sb
 * @param dest
 * @param size
 * @return int 1 success, 0 fail
 */
static int num = 0;
int rpc_deserialize_data(serialbuf_t* sb, char* dest, int size) {
	if (!sb || !sb->buffer) return 0;
	if (!size) return 0;
	if ((sb->capacity - sb->cursor) < size) return 0;

	memcpy(dest, sb->buffer + sb->cursor, size);

	sb->cursor += size;

	return 1;
}

/* getters */
/**
 * @brief Get the size of the given serialized buffer
 *
 * @param sb
 * @return int
 */
int rpc_get_serialbuf_size(serialbuf_t* sb) {
	return sb->capacity;
}

/**
 * @brief Get the current cursor offset of the given serialized buffer
 *
 * @param sb
 * @return int
 */
int rpc_get_serialbuf_offset(serialbuf_t* sb) {
	return !sb ? -1 : sb->cursor;
}

/**
 * @brief Get the current cursor pointer of the given serialized buffer
 *
 * @param sb
 * @return int
 */
char* rpc_get_serialbuf_cursor(serialbuf_t* sb) {
	return !sb ? NULL : sb->buffer + sb->cursor;
}

/* setters */

/**
 * @brief Skip the serialized buffer's cursor to the given position
 *
 * @param sb
 * @param skip_n
 * @return int 1 success, 0 fail
 */
int rpc_serialbuf_skip(serialbuf_t* sb, int skip_n) {
	int available_size = sb->capacity - sb->cursor;

	if (available_size >= skip_n) {
		sb->cursor += skip_n;
		return 1;
	}

	// double the buffer size until available >= skip_n
	while (available_size < skip_n) {
		sb->capacity = sb->capacity * 2;
		available_size = sb->capacity - sb->cursor;
	}

	// actually allocate the memory needed to satisfy amended size
	if (!(sb->buffer = realloc(sb->buffer, sb->capacity))) {
		return 0;
	}

	sb->cursor += skip_n;

	return 1;
}

/**
 * @brief Truncate the given buffer capacity to the end of the current cursor.
 *
 * Presumably, the consumer has ensured the cursor points to the actual end of the
 * stored data (as opposed to a skip position - this will truncate the data)
 *
 * @param sb
 */
void rpc_serialbuf_truncate(serialbuf_t** sb) {
	serialbuf_t* clone = NULL;

	// return if the buffer cursor is already at capacity
	if ((*sb)->cursor == (*sb)->capacity) {
		return;
	}

	rpc_serialbuf_init(&clone, (*sb)->cursor);
	memcpy(clone->buffer, (*sb)->buffer, (*sb)->cursor);
	clone->cursor = clone->capacity;
	rpc_serialbuf_free(*sb);
	*sb = clone;
}

/**
 * @brief Reset the serialized buffer's cursor
 *
 * @param sb
 */
void rpc_serialbuf_reset(serialbuf_t* sb) {
	sb->cursor = 0;
}

/* destructors */

/**
 * @brief Free heap memory allocated for a serialized buffer
 *
 * @param sb
 */
void rpc_serialbuf_free(serialbuf_t* sb) {
	free(sb->buffer);
	free(sb);
}

/**
 * @brief Copy data into the buffer at the given offset
 *
 * @param sb
 * @param size
 * @param value
 * @param offset
 * @return int
 */
int rpc_copy_at_offset(
	serialbuf_t* sb,
	int size,
	char* data,
	int offset
) {
	if (offset > sb->capacity){
		return 0;
	}

	memcpy(sb->buffer + offset, data, size);
	return 1;
}
