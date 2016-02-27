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
		// set of words hashes 
		typedef std::set<int> setData;
		// word hasher
		typedef std::hash<std::string> strHasher;

	public:
		// process input words
		bool ProcessInput(std::string data_file_path);

	};
}