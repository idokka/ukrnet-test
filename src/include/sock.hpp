#pragma once
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace ukrnet
{
	struct Sock
	{
		int desc;
		sockaddr_in addr;
		socklen_t addr_len;

		Sock() 
			: desc(0)
			, addr_len(sizeof(addr))
		{
			std::memset(&addr, 0, addr_len);
		}
	};
}