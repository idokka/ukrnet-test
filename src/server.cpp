#include <iostream>
#include <signal.h>
#include "include/logger.hpp"
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

	// try parse command line arguments
	try 
	{
		CmdLine cmd("server", ' ', "0.4");
		ValueArg<int> port_arg("p", "port", "Port to listen by server", true, port, "int");
		ValueArg<string> log_path_arg("l", "log", "Path to log file", false, log_path, "string");
		cmd.add(port_arg);
		cmd.add(log_path_arg);
		cmd.parse(argc, argv);
		// extract parsed values
		port = port_arg.getValue();
		log_path = log_path_arg.getValue();
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
	MemCached memcached(port);
	// register signals callback
	signal(SIGUSR1, memcached.GetSigUsr1Handler().target<void(int)>());
	signal(SIGUSR2, memcached.GetSigUsr2Handler().target<void(int)>());
	// run server
	memcached.Run();
	return 0;
}
