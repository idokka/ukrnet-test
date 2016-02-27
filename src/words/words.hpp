#pragma once
#include <set>
#include <string>
#include <functional>

namespace ukrnet
{
	// word list parser
	class WordsParser
	{
	public:
		// data record
		struct Record
		{
			// word hash
			int hash;
			// word
			std::string word;
			// default constructor
			Record(std::string word);
			// lesser for compare
			bool operator < (const Record &rv) const;
		};

		// set of word data 
		typedef std::set<Record> setData;
		// word hasher
		typedef std::hash<std::string> strHasher;

	public:
		// process input words
		bool ProcessInput(std::string data_file_path);

	};
}