#include <iostream>
#include "../include/logger.hpp"
#include "memcached.hpp"
#include "tclap/CmdLine.h"

using namespace std;
using namespace ukrnet;
using namespace TCLAP;

int main(int argc, char const *argv[])
{
	// set defaults
	int port = Server::DefaultPort;
	string log_path = Logger::DefaultLogPath();
	bool use_thread(false);
	bool use_fork(false);

	// try parse command line arguments
	try 
	{
		CmdLine cmd("server", ' ', "0.4");
		ValueArg<int> port_arg("p", "port", "Port to listen by server", false, port, "int");
		ValueArg<string> log_path_arg("l", "log", "Path to log file", false, log_path, "string");
		SwitchArg use_thread_arg("t","use-thread", "Use thread for connection execute", false);
		SwitchArg use_fork_arg("f","use-fork", "Use fork for connections execute", false);
		cmd.add(port_arg);
		cmd.add(log_path_arg);
		cmd.add(use_thread_arg);
		cmd.add(use_fork_arg);
		cmd.parse(argc, argv);
		// extract parsed values
		port = port_arg.getValue();
		log_path = log_path_arg.getValue();
		use_thread = use_thread_arg.getValue();
		use_fork = use_fork_arg.getValue();
	}
	catch (ArgException &e) 
	{ 
		// catch any parsing exceptions, write to std err
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl; 
	}

	// init logger
	logger().set_log_path(log_path);
	logger().init();

	// create server instance
	MemCached memcached(port, use_thread, use_fork);
	// register signals callback
	SigHandler::set_on_sigusr1(memcached.GetSigUsr1Handler());
	SigHandler::set_on_sigusr2(memcached.GetSigUsr2Handler());
	// run server
	memcached.Run();
	return 0;
}
