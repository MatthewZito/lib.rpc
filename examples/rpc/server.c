#include "librpc.h"

#include "common.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>

int main(int argc, char* argv[]) {
	int stream = 0,
	 reply_msg_size = 0,
					  	opt = 1,
								  len,
					   addr_len;

	struct sockaddr_in srv_addr, client_addr;

	if ((stream = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		panic("socket err");
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SRV_PORT);
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	addr_len = sizeof(struct sockaddr);

	if (setsockopt(
		stream,
		SOL_SOCKET,
		SO_REUSEADDR,
		(char*)&opt,
		sizeof(opt)
	) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(
		stream,
		SOL_SOCKET,
		SO_REUSEPORT,
		(char*)&opt,
		sizeof(opt)
	) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (bind(
		stream,
		(struct sockaddr*)&srv_addr,
		sizeof(struct sockaddr)
	) == -1) {
		panic("sock bind err");
	}

	// prepare buffers

	serialbuf_t* recvbuf = NULL,
						 * sendbuf = NULL;

	rpc_serialbuf_init(&recvbuf, MAX_BUFSIZE);
	rpc_serialbuf_init(&sendbuf, MAX_BUFSIZE);

	puts("Server ready to service RPC...");

	while (1) {
		rpc_serialbuf_reset(recvbuf);

		len = recvfrom(
			stream,
			// this will store data from clients
			recvbuf->buffer,
			rpc_get_serialbuf_size(recvbuf),
			MSG_WAITALL,
			(struct sockaddr*)&client_addr,
			&addr_len
		);

		printf("n bytes recv = %d\n", len);

		// prepare buffer for reply to be sent to the client
		rpc_serialbuf_reset(sendbuf);

		process_message(recvbuf, sendbuf);

		len = sendto(
			stream,
			sendbuf->buffer,
			rpc_get_serialbuf_size(sendbuf),
			MSG_CONFIRM,
			(struct sockaddr*)&client_addr,
			sizeof(struct sockaddr)
		);

		printf("n bytes sent = %d\n", len);
	}

	return EXIT_SUCCESS;
}

void marshal_srv(int res, serialbuf_t* sb) {
	rpc_serialize_data(sb, (char*)&res, sizeof(int));
}

void process_message(serialbuf_t* recvbuf, serialbuf_t* sendbuf) {
	rpc_header_t header;

	// header: optional but recommended
	rpc_deserialize_data(recvbuf, (char*)&header.id, sizeof(header.id));
	rpc_deserialize_data(recvbuf, (char*)&header.payload_size, sizeof(header.payload_size));

	int x, y, res;
	rpc_deserialize_data(recvbuf, (char*)&x, sizeof(int));
	rpc_deserialize_data(recvbuf, (char*)&y, sizeof(int));

	switch (header.id) {
		case RPC_MULTIPLY:
			res = x * y;
			break;
		case RPC_DIVIDE:
			{
				puts("HERE");
				if (y == 0) res = -1;
				else res = x / y;
			}
			break;
		case RPC_ADD:
			res = x + y;
			break;
		case RPC_SUBTRACT:
			res = x - y;
			break;
		default:
			res = -1;
	}

	marshal_srv(res, sendbuf);
}
