#ifndef RPC_DEMO_H
#define RPC_DEMO_H

#define MAX_BUFSIZE 2048
#define SRV_PORT 9000
#define SRV_IP "127.0.0.1"

#include <stdio.h>
#include <stdlib.h>

// optional but it's a good practice to have one
typedef struct rpc_header {
	int id;
	int payload_size;
} rpc_header_t;

typedef enum rpc_command {
	RPC_MULTIPLY = 1,
	RPC_DIVIDE,
	RPC_ADD,
	RPC_SUBTRACT,
} rpc_command_t;

int panic(char* msg) {
	fprintf(stdout, "%s\n", msg);
	exit(EXIT_FAILURE);
}

void marshal_srv(int res, serialbuf_t* sb);
void process_message(serialbuf_t* recvbuf, serialbuf_t* sendbuf);

void send_rpc(serialbuf_t* sendbuf, serialbuf_t* recvbuf);
serialbuf_t* marshal(int x, int y, int selection);
int unmarshal(serialbuf_t* sb);
int stub(int x, int y, int selection);

#endif
