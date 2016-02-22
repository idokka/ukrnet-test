# Ukrnet Test

This is test project for ukrnet.  
Based on specifications.

## dependencies

* sublime text 3 (for development)
* tclap (command line parser, included as submodule)
* tup (build system, included as submodule)
* libfuse-dev (for tup)

## memcached-like server specifications

some modifications from [official memcached protocol specifications](https://raw.githubusercontent.com/memcached/memcached/master/doc/protocol.txt)

process such commands:
* get (key)
* set (key expires? data-length data)
* delete (key)

server should be able to:
* work with many clients
* process many commands in one client request
* store data in memory and file (with `mmap`)
* save data to file in `/tmp` on signal `SIGUSR1` catching
* use c++11 standart
