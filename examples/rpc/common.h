#ifndef RPC_DEMO_H
#define RPC_DEMO_H

#define MAX_BUFSIZE 2048
#define SRV_PORT 9000
#define SRV_IP "127.0.0.1"

#include <stdio.h>
#include <stdlib.h>

int panic(char* msg) {
	fprintf(stdout, "%s\n", msg);
	exit(EXIT_FAILURE);
}

void marshal_srv(int res, serialbuf_t* sb);
int unmarshal_srv(serialbuf_t* sb);
int multiply(int x, int y);

void send_rpc(serialbuf_t* sendbuf, serialbuf_t* recvbuf);
serialbuf_t* marshal(int x, int y);
int unmarshal(serialbuf_t* sb);
int multiply_stub(int x, int y);

#endif
