#include "factory.hpp"
#include "client.hpp"
#include "server.hpp"
#include "sock.hpp"
#include <thread>

using namespace ukrnet;

// do create new client and execute it
void FactoryThread::Create(Sock sock, funcClientExecute exec)
{
	auto do_exec = std::bind(& FactoryThread::DoExec, this, 
		std::placeholders::_1, std::placeholders::_2);
	// start new thread to work with client socket stream
	std::thread(do_exec, sock, exec).detach();
}

// execute client functor
void FactoryThread::DoExec(Sock sock, funcClientExecute exec)
{
	Client client(sock);
	exec(client);
}

// do create new client and execute it
void FactoryFork::Create(Sock sock, funcClientExecute exec)
{
}
