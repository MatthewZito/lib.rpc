#include "librpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/* Test Utilities */

typedef struct musician musician_t;

typedef struct record {
	char name[32];
	int year;
	musician_t* musician;
} record_t;

struct musician {
	unsigned int releases[3];
	int age;
	int* height;
	unsigned int* net_worth_qua_year[3];
	record_t record;
	record_t top_records[3];
	struct musician* bandmate;
	struct musician* bandmates[3];
};

void serialize_record(record_t* obj, serialbuf_t* sb);
void serialize_musician(musician_t* obj, serialbuf_t* sb);
record_t* deserialize_record(serialbuf_t* sb);
musician_t* deserialize_musician(serialbuf_t* sb);
int panic(char* err_msg);

int panic(char* err_msg) {
	fprintf(stdout, "%s\n", err_msg);
	exit(EXIT_FAILURE);
}

void serialize_record(record_t* obj, serialbuf_t* sb) {

	// if data is NULL, surrogate it with the sentinel value
	RPC_SENTINEL_INSERTION_ROUTINE(obj, sb);

	if (!rpc_serialize_data(sb, (char*)obj->name, 32)) {
		panic("serialization error");
	}

	if (!rpc_serialize_data(sb, (char*)&obj->year, sizeof(int))) {
		panic("serialization error");
	}
	serialize_musician(obj->musician, sb);
}

void serialize_musician(musician_t* obj, serialbuf_t* sb) {
	unsigned int iteration = 0;
	unsigned int sentinel = 0xFFFFFFFF;

	// if data is NULL, surrogate it with the sentinel value
	// we always do this when first serializing an object
	RPC_SENTINEL_INSERTION_ROUTINE(obj, sb);

	for (; iteration < 3; iteration++) {
		if (!rpc_serialize_data(sb, (char*)&obj->releases[iteration], sizeof(unsigned int))) {
			panic("serialization error");
		}
	}

	if (!rpc_serialize_data(sb, (char*)&obj->age, sizeof(int))) {
		panic("serialization error");
	}

	if (obj->height) {
		if (!rpc_serialize_data(sb, (char*)obj->height, sizeof(int))) {
			panic("serialization error");
		}
	} else {
		if (!rpc_serialize_data(sb, (char*)&sentinel, sizeof(int))) {
			panic("serialization error");
		}
	}

	for (iteration = 0; iteration < 3; iteration++) {
		if (obj->net_worth_qua_year[iteration]) {
			if (!rpc_serialize_data(sb, (char*)obj->net_worth_qua_year[iteration], sizeof(unsigned int))) {
				panic("serialization error");
			}
		} else {
			if (!rpc_serialize_data(sb, (char*)&sentinel, sizeof(unsigned int))) {
				panic("serialization error");
			}
		}
	}

	serialize_record(&obj->record, sb);

	for (iteration = 0; iteration < 3; iteration++) {
		serialize_record(&obj->top_records[iteration], sb);
	}

	serialize_musician(obj->bandmate, sb);

	for (iteration = 0; iteration < 3; iteration++) {
		serialize_musician(obj->bandmates[iteration], sb);
	}
}

record_t* deserialize_record(serialbuf_t* sb) {
	RPC_SENTINEL_DETECTION_ROUTINE(sb);

	record_t* obj = malloc(sizeof(record_t));

	if (!obj) panic("malloc error");

	if (!rpc_deserialize_data(sb, (char*)obj->name, 32)) {
		panic("deserialization error");
	} else if (!rpc_deserialize_data(sb, (char*)&obj->year, sizeof(int))) {
		panic("deserialization error");
	}

	obj->musician = deserialize_musician(sb);

	return obj;
}

musician_t* deserialize_musician(serialbuf_t* sb) {
	int iteration = 0;
	unsigned int sentinel = 0;

	RPC_SENTINEL_DETECTION_ROUTINE(sb);

	musician_t* obj = malloc(sizeof(musician_t));

	for (; iteration < 3; iteration++) {
		if (!rpc_deserialize_data(sb, (char*)&obj->releases[iteration], sizeof(unsigned int))) {
			panic("deserialization error");
		}
	}

	if (!rpc_deserialize_data(sb, (char*)&obj->age, sizeof(int))) {
		panic("deserialization error");
	}

	// check pointer member for sentinel
	if (!rpc_deserialize_data(sb, (char*)&sentinel, sizeof(unsigned int))) {
		panic("deserialization error");
	}

	if (sentinel == RPC_SENTINEL) {
		obj->height = NULL;
	} else {
		if (!rpc_serialbuf_skip(sb, (int)(-1 * sizeof(unsigned int)))) {
			panic("skip error");
		}

		obj->height = malloc(sizeof(int));

		if (!rpc_deserialize_data(sb, (char*)obj->height, sizeof(int))) {
			panic("deserialization error");
		}
	}

	for (iteration = 0; iteration < 3; iteration++) {
		if (!rpc_deserialize_data(sb, (char*)&sentinel, sizeof(unsigned int))) {
			panic("deserialization error2");
		}

		if (sentinel == RPC_SENTINEL) {
			obj->net_worth_qua_year[iteration] = NULL;
		} else {
			if (!rpc_serialbuf_skip(sb, (int)(-1 * sizeof(unsigned int)))) {
				panic("skip error");
			}

			obj->net_worth_qua_year[iteration] = malloc(sizeof(int));

			if (!rpc_deserialize_data(sb, (char*)obj->net_worth_qua_year[iteration], sizeof(unsigned int))) {
				panic("deserialization error");
			}
		}
	}

	record_t* record = deserialize_record(sb);
	obj->record = *record; // shallow copy
	free(record);

	for (iteration = 0; iteration < 3; iteration++) {
		record = deserialize_record(sb);

		obj->top_records[iteration] = *record;

		free(record);
	}

	obj->bandmate = deserialize_musician(sb);

	for (iteration = 0; iteration < 3; iteration++) {
		obj->bandmates[iteration] = deserialize_musician(sb);
	}

	return obj;
}

void print_musician(musician_t* m1) {
	int iteration = 0;
	for (; iteration < 3; iteration++) {
		printf("m1.releases[iteration] %d\n", m1->releases[iteration]);
	}

	printf("m1->age %d\n", m1->age);
	printf("*m1->height %d\n", *m1->height);

	for (iteration = 0; iteration < 3; iteration++) {
		printf("*m1->net_worth_qua_year[iteration]");
		if (m1->net_worth_qua_year[iteration]) {
			printf(" %d\n", *m1->net_worth_qua_year[iteration]);
		} else printf(" null\n");
	}

	printf("m1->record %s %d\n", m1->record.name, m1->record.year);
	for (iteration = 0; iteration < 3; iteration++) {
		printf("m1->top_records[iteration] year");
		if (strlen(m1->top_records[iteration].name)) {
			printf(" %s\n", m1->top_records[iteration].name);
		} else printf(" null\n");
	}
}

/* Runner */
int main(int argc, char* argv[]) {
	puts("\ninitializing data...\n");

	musician_t m1;
	memset(&m1, 0, sizeof(musician_t));

	m1.releases[0] = 1968;
	m1.releases[1] = 1969;
	m1.releases[2] = 1971;
	m1.releases[3] = 1972;

	m1.age = 2021 - 1954;

	m1.height = malloc(sizeof(int));
	*m1.height = 170;

	m1.net_worth_qua_year[0] = NULL;
	m1.net_worth_qua_year[1] = NULL;
	m1.net_worth_qua_year[2] = malloc(sizeof(unsigned int));
	*m1.net_worth_qua_year[2] = 50000;

	strncpy(m1.record.name, "Tago Mago", strlen("Tago Mago"));
	m1.record.year = 1971;

	m1.record.musician = NULL;

	puts("\nserializing data...\n");

	serialbuf_t* sb;
	rpc_serialbuf_init(&sb, -1);

	// print OG object
	print_musician(&m1);

	// we recurse the object and serialize it into a flattened data structure
	serialize_musician(&m1, sb);

	// this is where we would send the serialized data over the wire...
	// but we're only testing serialization here

	// reset the buffer so we can read from start
	rpc_serialbuf_reset(sb);

	puts("\ndeserializing data...\n");

	musician_t* result = deserialize_musician(sb);

	rpc_serialbuf_free(sb);
	sb = NULL;

	print_musician(result);


	// VERIFY the two are *exactly* the same
	return EXIT_SUCCESS;
}
