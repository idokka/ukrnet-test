#include "words.hpp"
#include "../include/logger.hpp"
#include "../include/stream-helper.hpp"
#include <string>
#include <locale>
#include <fstream>
#include <iostream>
#include <functional>

using namespace ukrnet;

// empty constructor
WordsParser::Record::Record()
	: hash(0)
{
	// none of init
}

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

namespace ukrnet
{
	// read from stream operator
	std::istream & operator >> (std::istream &stream, WordsParser::Record &record)
	{
		size_t len(0);
		stream.read((char *)&record.hash, sizeof(int));
		stream.read((char *)&len, sizeof(size_t));
		record.word.resize(len, '\0');
		stream.read((char *)record.word.data(), len);
		return stream;
	}

	// write to stream operator
	std::ostream & operator << (std::ostream &stream, const WordsParser::Record &record)
	{
		size_t len = record.word.size();
		stream.write((char *)&record.hash, sizeof(int));
		stream.write((char *)&len, sizeof(size_t));
		stream.write((char *)record.word.data(), record.word.size());
		return stream;
	}
}

// process input words
bool WordsParser::ProcessInput(std::string data_file_path)
{
	setData data;
	if (LoadIndexFile(data_file_path, data) == false)
	{
		if (CreateIndexFile(data_file_path) == false)
			return false;
		LoadIndexFile(data_file_path, data);
	}

	std::locale loc("en_US.UTF-8");
	std::cin.imbue(loc);
	std::cout.imbue(loc);

	std::string word;
	do
	{
		std::getline(std::cin, word);
		if ((word.empty() == false) && (data.find(Record(word)) != data.end()))
			std::cout << word << std::endl;

	} while (std::cin.eof() == false);
}

// create index file for specified data file
bool WordsParser::CreateIndexFile(std::string data_file_path)
{
	std::ifstream text_data(data_file_path);
	if (text_data.is_open() == false)
	{
		logger().err("cannot open data file", data_file_path);
		return false;
	}

	std::string bin_data_file_path = data_file_path + ".bin";
	std::ofstream bin_data(bin_data_file_path, std::ios::binary);
	if (bin_data.is_open() == false)
	{
		logger().err("cannot open binary data file to write", bin_data_file_path);
		return false;
	}

	std::locale loc("en_US.UTF-8");
	text_data.imbue(loc);

	setData data;
	std::string word;
	do
	{
		std::getline(text_data, word);
		if (word.empty() == false)
			data.insert(Record(word));
	}
	while (text_data.eof() == false);
	text_data.close();

	for (const Record &record : data)
	{
		bin_data << record;
	}
	bin_data.flush();
	bin_data.close();

	logger().nfo("writed item count", data.size());
	return true;
}

// load index file
bool WordsParser::LoadIndexFile(std::string data_file_path, WordsParser::setData &data)
{
	std::string bin_data_file_path = data_file_path + ".bin";
	std::ifstream bin_data(bin_data_file_path, std::ios::binary);
	if (bin_data.is_open() == false)
	{
		logger().err("cannot open binary data file to read", bin_data_file_path);
		return false;
	}

	data.clear();
	do
	{
		Record record;
		bin_data >> record;
		if (record.hash != 0)
			data.insert(record);
	}
	while (bin_data.eof() == false);
	bin_data.close();
	return true;
}