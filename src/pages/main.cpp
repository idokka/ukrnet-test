#include <string>
#include <ostream>
#include <iostream>
#include "../include/logger.hpp"
#include "tclap/CmdLine.h"
#include "pages.hpp"

using namespace std;
using namespace ukrnet;
using namespace TCLAP;

int main(int argc, char const *argv[])
{
	// set defaults
	string data_file = "pages.bin";
	string log_path = "words.log";

	// try parse command line arguments
	try 
	{
		CmdLine cmd("pages", ' ', "0.1");
		ValueArg<string> data_file_arg("d", "data", "Data file path", false, data_file, "string", cmd);
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
	Pages pages(data_file);
	string cmd;
	int item(0);
	int page(0);
	int count(10);
	while (true)
	{
		cout 
			<< "0(c) set items count on page" << endl
			<< "1(gi) get item page" << endl
			<< "2(gp) get page" << endl
			<< "3(a) add item" << endl
			<< "4(d) delete item" << endl
			<< "5(e) check item exists" << endl
			<< "6(s) get stat" << endl
			<< "7(t) generate test data" << endl
			<< "8(q) quit" << endl
			<< "> ";
		cin >> cmd;
		if ((cmd == "c") || (cmd == "0"))
		{
			cin >> count;
			cout << ": page size setted to " << count << endl;
		}
		else if ((cmd == "gi") || (cmd == "1"))
		{
			cin >> item;
			int page = pages.GetItemPage(item, count);
			cout 
				<< ": item " << item 
				<< " placed on page " << page << endl;
		}
		else if ((cmd == "gp") || (cmd == "2"))
		{
			cin >> page;
			auto data = pages.GetPage(page, count);
			cout << ": page " << page << " items: ";
			for (int item : data)
				cout << item << " ";
			cout << endl;
		}
		else if ((cmd == "a") || (cmd == "3"))
		{
			cin >> item;
			bool result = pages.AddItem(item);
			cout 
				<< ": item " << item 
				<< (result ? " has been added" : " already exists") << endl;
		}
		else if ((cmd == "d") || (cmd == "4"))
		{
			cin >> item;
			bool result = pages.EraseItem(item);
			cout 
				<< ": item " << item 
				<< (result ? " has bees deleted" : " not exists") << endl;
		}
		else if ((cmd == "e") || (cmd == "5"))
		{
			cin >> item;
			bool result = pages.ItemExists(item);
			cout 
				<< ": item " << item 
				<< (result ? " exists" : " not exists") << endl;
		}
		else if ((cmd == "s") || (cmd == "6"))
		{
			cout 
				<< ": items count: " << pages.GetItemCount() 
				<< ", pages count: " << pages.GetPageCount(count)
				<< ", page size: " << count << endl;
		}
		else if ((cmd == "t") || (cmd == "7"))
		{
			pages.GenerateTestFile();
			cout << ": test file has been generated" << endl;
		}
		else if ((cmd == "q") || (cmd == "8"))
		{
			cout << ": goodbye" << endl;
			break;
		}
	}
	return 0;
}