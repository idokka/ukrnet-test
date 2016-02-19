#pragma once
#include <iomanip>

namespace ukrnet
{
	// put current date and time in ostream
	inline std::ostream& date_time(std::ostream& stream)
	{
		time_t tt = time(0);
		struct tm * now = localtime(&tt);
		stream
			<< "[" << std::setfill('0')
			<< std::setw(2) << now->tm_hour << ":"
			<< std::setw(2) << now->tm_min << "."
			<< std::setw(2) << now->tm_sec << " "
			<< std::setw(2) << now->tm_mday << '.' 
			<< std::setw(2) << (now->tm_mon + 1) << '.'
			<< std::setw(4) << (now->tm_year + 1900)
			<< "]" << std::setfill(' ');
		return stream;
	}

	// put tab in ostream
	inline std::ostream& tab(std::ostream& stream)
	{
		return stream << '\t';
	}
}