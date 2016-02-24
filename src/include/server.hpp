#pragma once
#include <functional>
#include "client.hpp"
#include "sock.hpp"

namespace ukrnet
{
	// socket server
	class Server
	{
	public:
		// custom client functor
		typedef std::function<void(Client &)> funcClient;

	public:
		// default server port
		static const int DefaultPort = 11222;

	public:
		// default constructor
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

	private:
		// main client function
		void DoClientExecute(Sock sock);

	public:
		// get socket port
		int port() const { return _port; }
		// set socket port, returns operation result
		bool set_port(int value) { if (_is_opened) return false; else return (_port = value); }
		// get is server opened
		bool is_opened() const { return _is_opened; }
		// set is server opened, returns operation result
		bool set_is_opened(bool value) { return value ? Open() : Close(); }
		// get client socket function
		const funcClient &client_func() const { return _client_func; }
		// set client socket function
		void set_client_func(funcClient value) { _client_func = value; }

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
