CC=gcc
CFLAGS=-g -fPIC
LDFLAGS=-shared -o

WIN_BIN=lib_rpc.dll
UNIX_BIN=librpc.so

OBJFILES=$(wildcard src/*.c)

all: unix

unix:
	$(CC) $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(UNIX_BIN)

win:
	$(CC) $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(WIN_BIN)

clean:
	rm $(TARGET)
