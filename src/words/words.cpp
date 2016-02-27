#include "words.hpp"
#include "../include/logger.hpp"
#include "../include/stream-helper.hpp"
#include <string>
#include <locale>
#include <fstream>
#include <iostream>

using namespace ukrnet;

// generate binary data file from text data file
bool WordsParser::ProcessInput(std::string data_file_path)
{
	std::ifstream in_stream(data_file_path);
	if (in_stream.is_open() == false)
	{
		logger().err("cannot open data file", data_file_path);
		return false;
	}
	std::locale loc("en_US.UTF-8");
	in_stream.imbue(loc);

	setData data;
	strHasher hash;
	std::string word;
	do
	{
		std::getline(in_stream, word);
		if (word.empty() == false)
			data.insert(hash(word));
	}
	while (in_stream.eof() == false);
	in_stream.close();
	logger().nfo("readed from data file", data_file_path, data.size());

	std::cin.imbue(loc);
	std::cout.imbue(loc);

	do
	{
		std::getline(std::cin, word);
		if ((word.empty() == false) && (data.find(hash(word)) != data.end()))
			std::cout << word << std::endl;
		else
			break;

	} while (std::cin.eof() == false);
}
