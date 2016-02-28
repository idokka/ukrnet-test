#include <iostream>
#include "../include/logger.hpp"
#include "tclap/CmdLine.h"
#include "words.hpp"

using namespace std;
using namespace ukrnet;
using namespace TCLAP;

int main(int argc, char const *argv[])
{
	// set defaults
	string data_file = "words.txt";
	string log_path = "words.log";

	// try parse command line arguments
	try 
	{
		CmdLine cmd("words", ' ', "0.2");
		ValueArg<string> data_file_arg("s", "src", "Data file path", false, data_file, "string", cmd);
		ValueArg<string> log_path_arg("l", "log", "Log file path", false, log_path, "string", cmd);
		cmd.parse(argc, argv);
		// extract parsed values
		data_file = data_file_arg.getValue();
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

	// do work
	WordsParser words;
	words.ProcessInput(data_file);
	return 0;
}
