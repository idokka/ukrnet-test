#pragma once
#include <unistd.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "logger.hpp"

namespace ukrnet
{
	namespace server
	{
		class Server
		{
		public:
			Server(int port)
				: _port(port)
			{

			}
			
			~Server()
			{

			}

			bool open()
			{
				// try create socket
				_sock_file_d = socket(AF_INET, SOCK_STREAM, 0);
				if (_sock_file_d < 0)
				{
					logger().err("server", "cannot open descriptor", errno);
					return false;
				}

				// clear addr struct
				memset(&_server_addr, 0, sizeof(sockaddr_in));
				_server_addr.sin_family = AF_INET;
				_server_addr.sin_addr.s_addr = INADDR_ANY;
				_server_addr.sin_port = htons(_port);

				// try bind socket
				int error = bind(_sock_file_d, (const struct sockaddr *) &_server_addr, sizeof(_server_addr));
				if (error < 0)
				{
					logger().err("server", "cannot bind socket", errno);
					return false;
				}

				// try listen socket
				error = listen(_sock_file_d, 5);
				if (error < 0)
				{
					logger().err("server", "cannot listen socket", errno);
					return false;
				}

				logger().nfo("server", "socket opened", _port);
				return true;
			}
		private:
			int _port;
			int _sock_file_d;
			sockaddr_in _server_addr;
			sockaddr_in _client_addr;

		};
	}
}
