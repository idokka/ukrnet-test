#pragma once
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace ukrnet
{
	// summary structures for socket
	struct Sock
	{
		// socket file descriptor
		int desc;
		// socket in address struct
		sockaddr_in addr;
		// socket in address struct length
		socklen_t addr_len;
		// default constructor
		Sock() 
			: desc(0)
			, addr_len(sizeof(addr))
		{
			std::memset(&addr, 0, addr_len);
		}
	};
}