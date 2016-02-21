#include <iostream>
#include <functional>
#include <signal.h>
#include "include/logger.hpp"
#include "memcached.hpp"
#include "tclap/CmdLine.h"

using namespace std;
using namespace ukrnet;
using namespace TCLAP;

struct SignalHandlers
{
	static std::function<void(int)> & on_sigusr1()
	{
		static std::function<void(int)> _on_sigusr1;
		return _on_sigusr1;
	}

	static std::function<void(int)> & on_sigusr2()
	{
		static std::function<void(int)> _on_sigusr2;
		return _on_sigusr2;
	}

	static void sigusr1(int signum)
	{
		if (on_sigusr1())
			on_sigusr1()(signum);
	}

	static void sigusr2(int signum)
	{
		if (on_sigusr2())
			on_sigusr2()(signum);
	}

	static void do_register()
	{
		signal(SIGUSR1, sigusr1);
		signal(SIGUSR2, sigusr2);
	}
};

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
	SignalHandlers::do_register();
	SignalHandlers::on_sigusr1() = memcached.GetSigUsr1Handler();
	SignalHandlers::on_sigusr2() = memcached.GetSigUsr2Handler();
	// run server
	memcached.Run();
	return 0;
}
