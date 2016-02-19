#pragma once
#include <cerrno>
#include <thread>
#include <functional>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "logger.hpp"
#include "client.hpp"
#include "sock.hpp"
#include "client.hpp"

namespace ukrnet
{
	// socket server
	class Server
	{
	public:
		// custom client functor
		typedef std::function<void(Client &)> funcClient;
	public:
		// default constructor
		// port: server port for listen
		// client_func: custom client functor
		Server(int port, funcClient client_func)
			: _port(port)
			, _is_opened(false)
		{
		}
		
		// default destructor; closes socket if needed
		~Server()
		{
			Close();
		}

		// open socket
		// returns true, if socket has successfuly opened, otherwise false
		bool Open()
		{
			// check if socket already is opened
			if (_is_opened)
				return true;

			// try create socket
			_sock.desc = socket(AF_INET, SOCK_STREAM, 0);
			if (_sock.desc < 0)
			{
				logger().err("server", "cannot open descriptor", errno);
				return false;
			}

			// fill addr struct
			_sock.addr.sin_family = AF_INET;
			_sock.addr.sin_addr.s_addr = INADDR_ANY;
			_sock.addr.sin_port = htons(_port);

			// try bind socket
			int error = bind(_sock.desc, (const struct sockaddr *) &_sock.addr, sizeof(_sock.addr_len));
			if (error < 0)
			{
				logger().err("server", "cannot bind socket", errno);
				return false;
			}

			// try listen socket
			error = listen(_sock.desc, 5);
			if (error < 0)
			{
				logger().err("server", "cannot listen socket", errno);
				return false;
			}

			// socket opening successful
			logger().nfo("server", "socket opened", _port);
			_is_opened = true;
			return true;
		}

		// closes socket
		// returns true, if socket has successfuly closed, otherwise false
		bool Close()
		{
			// check if socket is already closed
			if (_is_opened == false)
				return true;

			// close socket
			int error = close(_sock.desc);
			if (error < 0)
			{
				logger().err("server", "cannot close socket", errno);
				return false;
			}

			// socket closing successful
			_is_opened = false;
			return true;
		}

		// main accpet loop; creates thread on every incloming connection, call client functor
		bool Accept()
		{
			while (true)
			{
				Sock client;
				client.desc = accept(_sock.desc, (struct sockaddr *) &client.addr, &client.addr_len);
				// start new thread to work with client socket stream
				auto do_client_execute = std::bind(& Server::DoClientExecute, this, std::placeholders::_1);
				std::thread(do_client_execute, client);
			}
		}

	private:
		void DoClientExecute(Sock sock)
		{
			Client client(sock);
			_client_func(client);
		}

	public:
		// get is server opened
		bool is_opened() const { return _is_opened; }
		// set is server opened, returns operation result
		bool set_is_opened(bool value) { return value ? Open() : Close(); }

	private:
		// listen port
		int _port;
		// server socket struct
		Sock _sock;
		// is socket opened
		bool _is_opened;
		// client execution function
		funcClient _client_func;

	};
}
