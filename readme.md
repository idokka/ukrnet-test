# Ukrnet Test

This is test project for ukrnet. Based on [specifications](specs.pdf).

## Dependencies

* Sublime Text 3 (for development);
* [TCLAP](http://tclap.sourceforge.net/) (Templatized C++ Command Line Parser, included as submodule);
* [Tup](http://gittup.org/tup/) (build system, included as submodule);
* libfuse-dev (for Tup);

## Using

```shell
# clone repository
~ $ git clone git@github.com:idokka/ukrnet-test.git
~ $ cd ukrnet-test
~ $ git submodule init
~ $ git submodule update
# init tup
tup $ cd thirdparty/tup
tup $ ./bootstrap.sh
tup $ sudo ln ~/ukrnet-test/thirdparty/tup/tup /usr/bin/tup
# init tclap
tclap $ cd ../tclap
tclap $ ./configure
tclap $ make
tclap $ make install
src $ cd ../../src
src $ tup
# get ready to testing
src $ bin/server &
src $ telnet localhost 11222
set testkey 9
testvalue
STORED
get testkey
VALUE testkey 9
testvalue
END
del testkey
DELETED
del wrongkey
NOT_FOUND
get wrongkey
END
src $ killall server
# test number paging manager
src $ bin/pages
...
# test words parser
src $ cat in.txt | bin/words
...
```

## Task 1: Memcached-like server

Some modifications from [official memcached protocol specifications](https://raw.githubusercontent.com/memcached/memcached/master/doc/protocol.txt).

Process such commands:
* get (key)
* set (key expires? data-length data)
* delete (key)

`key` - string without special symbols and any spaces. Used as data for hasher and integer key in dictionary. `data` - any bytes data with .

Server should be able to:
* Work with many clients;
* Process many commands in one client request;
* Store data in memory and file (with `mmap`);
* Save data to file in `/tmp` on signal `SIGUSR1` catching;
* Use c++11 standart;
* Support different client connection execution models;

### Command `get` [cpp:242](src/memcached/memcached.cpp#L242)

Query syntax: `get key\r\n`. Servers search this key in map and answer to client in that way: `VALUE key size\r\ndata\r\nEND`.

### Command `set` [cpp:177](src/memcached/memcached.cpp#L177)

Query syntax: `set key expires? data-length\r\ndata\r\n`. Server add specified data to map with specified key. Expires is optional parameter, that specified seconds count to store this key-data. If key exists data would be overwrited. Server answer syntax: `STORED` if operation has been successful, or `EXISTS` if specified key is already exists.

### Command `del` [cpp:277](src/memcached/memcached.cpp#L277)

Query syntax: `del key`. Server find specified key in map and delete it. Server ansver syntax: `DELETED` if operation has been successful, or `NOT_FOUND` if key not found in map. 

### Server command line arguments

Usage: `server [-f] [-t] [-l <string>] [-p <int>] [--] [--version] [-h]`.
Where:
* `-f` or `--use-fork` - Use fork connections execute model; default is unsetted;
* `-t` or `--use-thread` - Use thread connection execute model; defaul is setted;
* `-l <string>` or `--log <string>` - Path to log file; default is `server.log`;
* `-p <int>` or `--port <int>` - Port to listen by server socket; default is `11222`;

## Task 2.1: Words parser

Application write to output word from input, that contains in specified text file.

## Task 2.2: Numbers paging

Application stores unique numbers, that sorted in descedent order. Application uses file-based binary storage.