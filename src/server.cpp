#include <iostream>
#include "include/logger.hpp"
#include "memcached.hpp"
#include "tclap/CmdLine.h"

using namespace std;
using namespace ukrnet;
using namespace TCLAP;

// default server port
const int default_port = 11222;
// default log file
const char *default_log_path = "server.log";

int main(int argc, char const *argv[])
{
	// set defaults
	int port = default_port;
	string log_path = default_log_path;

	// try parse command line arguments
	try 
	{
		CmdLine cmd("server", ' ', "0.2");
		ValueArg<int> port_arg("p", "port", "Port to listen by server", true, default_port, "int");
		ValueArg<string> log_path_arg("l", "log", "Path to log file", false, default_log_path, "string");
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

	MemCached memcached(port);
	memcached.Run();
	return 0;
}
