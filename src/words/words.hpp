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
			// empty constructor
			Record();
			// default constructor
			Record(std::string word);
			// lesser for compare
			bool operator < (const Record &rv) const;
			// read from stream operator
			friend std::istream & operator >> (std::istream &stream, Record &record);
			// write to stream operator
			friend std::ostream & operator << (std::ostream &stream, const Record &record);
		};

		// set of word data 
		typedef std::set<Record> setData;
		// word hasher
		typedef std::hash<std::string> strHasher;

	public:
		// process input words
		bool ProcessInput(std::string data_file_path);

	private:
		// create index file for specified data file
		bool CreateIndexFile(std::string data_file_path);
		// load index file
		bool LoadIndexFile(std::string data_file_path, setData &data);

	};
}