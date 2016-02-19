#pragma once
#include <functional>
#include "include/server.hpp"
#include "include/client.hpp"
#include "include/logger.hpp"

namespace ukrnet
{
	// test MemCached server
	class MemCached
	{
	public:
		// default constructor
		// port: server socket port
		MemCached(int port)
			: _port(port)
		{
			// none of init
		}

		// do run server
		void Run()
		{
			// start server
			_server.set_port(_port);
			_server.set_client_func(GetClientFunc());
			_server.Open();
			if (_server.is_opened() == false)
			{
				logger().err("main", "failed to open server");
				return;
			}

			// start server listen
			_server.Accept();
		}

	private:
		// construct client func
		Server::funcClient GetClientFunc()
		{
			return std::bind(& MemCached::ClientFunc, this, std::placeholders::_1);
		}

		// client func
		void ClientFunc(Client &client)
		{
			client.Write("Hello from test!");
			client.WriteEndl();
			std::string cmd = client.Read();
			client.Write("This is your command:");
			client.Write(cmd);
			client.WriteEndl();
		}

	private:
		// server socket port
		int _port;
		// server socket
		Server _server;

	};
}