#include "server.hpp"
#include "logger.hpp"
#include "factory.hpp"

#include <memory>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace ukrnet;

// default constructor
Server::Server()
	: _port(0)
	, _is_opened(false)
	, _client_factory(nullptr)
{
	// none of init
}

// default destructor; closes socket if needed
Server::~Server()
{
	Close();
}

// open socket
// returns true, if socket has successfuly opened, otherwise false
bool Server::Open()
{
	// check if socket already is opened
	if (_is_opened)
		return true;
	// check is socket port is setted
	if (_port == 0)
	{
		logger().err("server", "port not specified");
		return false;
	}

	// try create socket
	_sock.desc = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock.desc < 0)
	{
		logger().err("server", "cannot open descriptor", curr_errno_msg());
		return false;
	}

	// fill addr struct
	_sock.addr.sin_family = AF_INET;
	_sock.addr.sin_addr.s_addr = INADDR_ANY;
	_sock.addr.sin_port = htons(_port);

	// try bind socket
	int error = bind(_sock.desc, (const struct sockaddr *) &_sock.addr, _sock.addr_len);
	if (error < 0)
	{
		logger().err("server", "cannot bind socket", curr_errno_msg());
		return false;
	}

	// try listen socket
	error = listen(_sock.desc, 5);
	if (error < 0)
	{
		logger().err("server", "cannot listen socket", curr_errno_msg());
		return false;
	}

	// socket opening successful
	logger().nfo("server", "socket opened", _port);
	_is_opened = true;
	return true;
}

// closes socket
// returns true, if socket has successfuly closed, otherwise false
bool Server::Close()
{
	// check if socket is already closed
	if (_is_opened == false)
		return true;

	// close socket
	int error = close(_sock.desc);
	if (error < 0)
	{
		logger().err("server", "cannot close socket", curr_errno_msg());
		return false;
	}

	// socket closing successful
	_is_opened = false;
	return true;
}

// main accept loop; creates thread on every incloming connection, call client functor
bool Server::Accept()
{
	if (_is_opened == false)
	{
		logger().err("server", "socket not opened");
		return false;
	}
	while (true)
	{
		Sock client;
		logger().nfo("server", "wait for connections");
		client.desc = accept(_sock.desc, (struct sockaddr *) &client.addr, &client.addr_len);
		logger().nfo("server", "new connection accepted");
		// call factory to work with client socket stream
		_client_factory->Create(client, _client_execute_func);
	}
}
