# lib.rpc

`lib.rpc` is a C library that describes a data exchange format for Remote Procedure Calls,
and exposes a public API for socket-based, multi-node RPC

I'm still working on this project; check back in a few days!

## Dynamic Linking

Linking to `lib.rpc`:

```bash
# 1) include and use lib.rpc in your project
# 2) generate object file for your project
gcc -I ../path/to/librpc -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to lib.rpc
gcc -o main main.o -L../path/to/librpc -llibrpc
# you may need to add the lib location to your PATH
```

Linking to `lib.rpc` on Windows:

```bash
# 1) include and use lib.rpc in your project
# 2) generate object file for your project
gcc -I ../path/to/librpc -c main.c -o main.o
# 3) generate shared object file
make win
# 3) link your project to lib.rpc
gcc -o main.exe main.o -L /path/to/lib.rpc -llib_rpc.dll
# you may need to add the lib location to your PATH
```

## Current Limitations

- cannot serialize/deserialize objects with back-pointers (e.g. circular linked lists, graphs, et al)
- the first member of a struct cannot be NULL
