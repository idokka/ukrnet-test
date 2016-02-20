#pragma once
#include <functional>
#include <sstream>
#include "include/server.hpp"
#include "include/client.hpp"
#include "include/logger.hpp"

namespace ukrnet
{
	// test MemCached server
	class MemCached
	{
	public:
		// server command
		enum class Cmd
		{
			None,
			Get,
			Set,
			Del
		};

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
			Cmd cmd(Cmd::None);
			std::string key;

			while (client.CheckIsAlive())
			{
				std::stringstream ss(client.Read());
				cmd = ParseCmd(ss);
				if (cmd == Cmd::Set)
					DoSet(client, ss);
				else if (cmd == Cmd::Get)
					DoGet(client, ss);
				else if (cmd == Cmd::Del)
					DoDel(client, ss);
			}
		}

		// parse cmd from stream
		Cmd ParseCmd(std::istream &ss)
		{
			std::string cmd;
			ss >> cmd;
			if (cmd == "get")
				return Cmd::Get;
			else if (cmd == "set")
				return Cmd::Set;
			else if (cmd == "delete")
				return Cmd::Del;
			else
				return Cmd::None;
		}

		void DoSet(Client &client, std::istream &ss)
		{
			std::string key;
			ss >> key;
			logger().nfo("main", "set recieved", key);
		}

		void DoGet(Client &client, std::istream &ss)
		{
			std::string key;
			ss >> key;
			logger().nfo("main", "get recieved", key);
		}

		void DoDel(Client &client, std::istream &ss)
		{
			std::string key;
			ss >> key;
			logger().nfo("main", "del recieved", key);
		}

	private:
		// server socket port
		int _port;
		// server socket
		Server _server;

	};
}