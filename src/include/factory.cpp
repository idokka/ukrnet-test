#include "factory.hpp"
#include "client.hpp"
#include "server.hpp"
#include "sock.hpp"
#include "logger.hpp"
#include <thread>
#include <unistd.h>

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
	pid_t child_process = fork();
	if (child_process == (pid_t)-1) 
	{
		logger().err("client", "unable to fork for client connection", curr_errno_msg());
		exit(1);
	}
	else if (child_process == 0)
	{
		logger().nfo("client", "forked for client connection");
		Client client(sock);
		exec(client);
	}
}
