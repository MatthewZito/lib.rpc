#include "librpc.h"

#include "common.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <memory.h>

/* Client Demo (start this after server.c) */
int main(int argc, char* argv[]) {
	int x, y, selection;

	puts("Multiply (1)\nDivide (2)\nAdd (3)\nSubtract (4)");
	scanf("%d", &selection);

	if (selection < 1 || selection > 4) {
		panic("invalid selection");
	}

	puts("enter x integer: ");
	scanf("%d", &x);
	puts("enter y integer: ");
	scanf("%d", &y);

	// the stub triggers the entire RPC cycle
	printf("result = %d\n", stub(x, y, selection));

	return EXIT_SUCCESS;
}

void send_rpc(serialbuf_t* sendbuf, serialbuf_t* recvbuf) {
	struct sockaddr_in dest;
	int stream;
	int addr_len;

	dest.sin_family = AF_INET;
	dest.sin_port = htons(SRV_PORT);
	struct hostent* host = (struct hostent*)gethostbyname(SRV_IP);
  dest.sin_addr = *((struct in_addr*)host->h_addr);
	addr_len = sizeof(struct sockaddr);

	stream = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (stream == -1) {
		perror("socket creation ");
    exit(EXIT_FAILURE);
	}

	int rc = sendto(
		stream,
		sendbuf->buffer,
		rpc_get_serialbuf_size(sendbuf),
    MSG_CONFIRM,
		(struct sockaddr*)&dest,
		sizeof(struct sockaddr)
	);

  printf("client: %d bytes sent\n", rc);

  int recv_size = recvfrom(
		stream,
		recvbuf->buffer,
    rpc_get_serialbuf_size(recvbuf),
		MSG_WAITALL,
    (struct sockaddr*)&dest,
		&addr_len
	);

  printf("client: %d bytes recv\n", recv_size);

	close(stream);
}

serialbuf_t* marshal(int x, int y, int selection) {
	serialbuf_t* sb = NULL;
	rpc_serialbuf_init(&sb, MAX_BUFSIZE);

	// reserve header space
	rpc_serialbuf_skip(sb, sizeof(int) * 2);

	// prepare header
	rpc_header_t header;
	header.id = selection;

	// we don't know this yet
	header.payload_size = 0;

	// serialize the data
	rpc_serialize_data(sb, (char*)&x, sizeof(int));
	rpc_serialize_data(sb, (char*)&y, sizeof(int));

	header.payload_size = rpc_get_serialbuf_size(sb) - (sizeof(int) * 2);

	if (!rpc_copy_at_offset(
		sb,
		sizeof(header.id),
		(char*)&header.id,
		0
	)) panic("serialization err");

	if (!rpc_copy_at_offset(
		sb,
		sizeof(header.payload_size),
		(char*)&header.payload_size,
		sizeof(header.id)
	)) panic("serialization err");

	return sb;

	// serialbuf looks like:
	/*

	+===+======+======+
	|		|      |      |
 |	h	|  x   |  y   |
  +===+======+======+
 <-8----4---+---4--->
	*/
}

int unmarshal(serialbuf_t* sb) {
	int res = 0;
	rpc_deserialize_data(sb, (char*)&res, sizeof(int));
	return res;
}

int stub(int x, int y, int selection) {
	serialbuf_t* sendbuf = marshal(x, y, selection);
	serialbuf_t* recvbuf = NULL;

	// prep to recv data from srv
	rpc_serialbuf_init(&recvbuf, MAX_BUFSIZE);

	// send serialized data to srv and await a reply
	send_rpc(sendbuf, recvbuf);

	// clear send buffer
	rpc_serialbuf_free(sendbuf);
	sendbuf = NULL;

	// srv will reply with result
	int res = unmarshal(recvbuf);

	rpc_serialbuf_free(recvbuf);
	recvbuf = NULL;

	return res;
}
