#ifndef LIB_RPC_H
#define LIB_RPC_H

#define RPC_SERIALBUF_SIZE 512

/* SENTINEL */

#define RPC_SENTINEL 0xFFFFFFFF
#define RPC_SENTINEL_INSERTION_ROUTINE(obj, sb) {                     \
	if (!obj) {                                                         \
		unsigned int sentinel = RPC_SENTINEL;                             \
		rpc_serialize_data(sb, (char*)&sentinel, sizeof(unsigned int));   \
		return;                                                           \
	}                                                                   \
}

#define RPC_SENTINEL_DETECTION_ROUTINE(sb) {                          \
	unsigned int sentinel = 0;                                          \
	rpc_deserialize_data(sb, (char*)&sentinel, sizeof(unsigned int));   \
	if (sentinel == RPC_SENTINEL) return NULL;                          \
	rpc_serialbuf_skip(sb, (int)(-1 * sizeof(unsigned int)));           \
}

typedef struct serialized_buffer {
	/* Internal buffer - implemented as a data stream */
	void* buffer;

	/* Current max size of the buffer (including memory not yet utilized for data */
	int capacity;

	/* Current position of the buffer where the next read / write will occur (NOT necessarily empty) */
	int cursor;
} serialbuf_t;

/* initializers */
void rpc_serialbuf_init(serialbuf_t** sb, int size);

/* serializers */
int rpc_serialize_data(serialbuf_t* sb, char* data, int size);
int rpc_deserialize_data(serialbuf_t* sb, char* dest, int size);
int rpc_copy_at_offset(
	serialbuf_t* sb,
	int size,
	char* data,
	int offset
);

/* getters */
int rpc_get_serialbuf_size(serialbuf_t* sb);
int rpc_get_serialbuf_offset(serialbuf_t* sb);
char* rpc_get_serialbuf_cursor(serialbuf_t* sb);

/* setters */
int rpc_serialbuf_skip(serialbuf_t* sb, int size);
void rpc_serialbuf_truncate(serialbuf_t** sb);
void rpc_serialbuf_reset(serialbuf_t* sb);

/* destructors */
void rpc_serialbuf_free(serialbuf_t* sb);

#endif /* LIB_RPC_H */
