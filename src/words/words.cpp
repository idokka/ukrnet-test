#include "words.hpp"
#include "../include/logger.hpp"
#include "../include/stream-helper.hpp"
#include <string>
#include <locale>
#include <fstream>
#include <iostream>
#include <functional>

using namespace ukrnet;

// default constructor
WordsParser::Record::Record(std::string word)
	: hash(0)
	, word(word)
{
	static strHasher hasher;
	hash = hasher(word);
}

// lesser for compare
bool WordsParser::Record::operator < (const WordsParser::Record &rv) const
{
	if (this->hash != rv.hash)
		return this->hash < rv.hash;
	else 
		return std::lexicographical_compare(
			this->word.begin(), this->word.end(),
			rv.word.begin(), rv.word.end());
}

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
			data.insert(Record(word));
	}
	while (in_stream.eof() == false);
	in_stream.close();
	logger().nfo("readed from data file", data_file_path, data.size());

	std::cin.imbue(loc);
	std::cout.imbue(loc);

	do
	{
		std::getline(std::cin, word);
		if ((word.empty() == false) && (data.find(Record(word)) != data.end()))
			std::cout << word << std::endl;
		else
			break;

	} while (std::cin.eof() == false);
}
