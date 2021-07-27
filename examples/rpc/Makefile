CC=gcc
OBJFILES=$(wildcard ./*.c)
INTERMEDIARY=main

all: client server

server: 
	$(CC) -I ../../src server.c -o server -L../../ -lrpc

client: 
	$(CC) -I ../../src client.c -o client -L../../ -lrpc

clean:
	rm client server
