#pragma once
#include <memory>
#include <functional>
#include "sock.hpp"
#include "client.hpp"
#include "factory.hpp"

namespace ukrnet
{
	// socket server
	class Server
	{
	public:
		// default server port
		static const int DefaultPort = 11222;

	public:
		// default constructor
		// port: server socket port
		// client_factory: factory of client execution model
		Server();
		// default destructor; closes socket if needed
		~Server();

		// open socket
		// returns true, if socket has successfuly opened, otherwise false
		bool Open();
		// closes socket
		// returns true, if socket has successfuly closed, otherwise false
		bool Close();
		// main accept loop; creates thread on every incloming connection, call client functor
		bool Accept();

	public:
		// get socket port
		int port() const;
		// set socket port, returns operation result
		bool set_port(int value);
		// get is server opened
		bool is_opened() const;
		// set is server opened, returns operation result
		bool set_is_opened(bool value);
		// get client socket function
		const IFactory::funcClientExecute &client_execute_func() const;
		// set client socket function
		void set_client_execute_func(IFactory::funcClientExecute value);
		// get factory for client execution model
		std::shared_ptr<IFactory> client_factory() const;
		// set factory for client execution model
		void set_client_factory(std::shared_ptr<IFactory> value);

	private:
		// listen port
		int _port;
		// server socket struct
		Sock _sock;
		// is socket opened
		bool _is_opened;
		// client execution function
		IFactory::funcClientExecute _client_execute_func;
		// factory of client execution
		std::shared_ptr<IFactory> _client_factory;

	};
}
