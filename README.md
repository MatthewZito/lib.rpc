# lib.rpc

`lib.rpc` is a C library that describes a data exchange format for Remote Procedure Calls,
and exposes a public API for socket-based, multi-node RPC

I'm still working on this project; check back in a few days!

## Serialization Format

The `lib.rpc` serialization format is a binary representation of the original struct data. The data is buffered into a stream; each unique object is delimited by a cursor (pointer to the memory offset). All `NULL` data is supplanted with a sentinel identifier (`0xFFFFFFFF`); this format does not support back-pointers, currently (e.g. no graphs or circular linked lists - fixing this in an upcoming release but by all means open a PR if you beat me to it).

To serialize any object, simply write a function which calls `rpc_serialize_data` on each member of the given structure; for nested structures, you may recurse and reference its serialization method.

Finally, initialize a buffer (`serialbuf_t**`) with `rpc_serialbuf_init` and serialize your data thereon.

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
