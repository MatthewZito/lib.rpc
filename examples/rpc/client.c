#include "librpc.h"

#include "common.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <memory.h>

/* Client Demo (start this after server.c) */
int main(int argc, char* argv[]) {
	int x, y;

	puts("enter x integer: ");
	scanf("%d", &x);
	puts("enter y integer: ");
	scanf("%d", &y);

	// the stub triggers the entire RPC cycle
	printf("result = %d\n", multiply_stub(x, y));

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

serialbuf_t* marshal(int x, int y) {
	serialbuf_t* sb = NULL;
	rpc_serialbuf_init(&sb, MAX_BUFSIZE);

	rpc_serialize_data(sb, (char*)&x, sizeof(int));
	rpc_serialize_data(sb, (char*)&y, sizeof(int));

	return sb;

	// serialbuf looks like:
	/*

	+======+======+
	|      |      |
	|  x   |  y   |
	+======+======+
 <---4---+---4--->
	*/
}

int unmarshal(serialbuf_t* sb) {
	int res = 0;
	rpc_deserialize_data(sb, (char*)&res, sizeof(int));
	return res;
}

int multiply_stub(int x, int y) {
	serialbuf_t* sendbuf = marshal(x, y);
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
