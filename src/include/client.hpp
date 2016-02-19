#pragma once
#include <array>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "logger.hpp"
#include "sock.hpp"

namespace ukrnet
{
	class Client
	{
	public:
		typedef std::array<char, 255> aBuffer;
	public:
		// default constructor
		// sock: Sock structure for client socket
		Client(Sock sock)
			: _sock(sock)
			, _delim(Delim())
			, _pos_begin(_buffer.begin())
			, _pos_end(_buffer.begin())
		{
			_buffer.fill('\0');
		}

		// destructor; close socket, if needed
		~Client()
		{
			close(_sock.desc);
		}

		// disable copy constructor and copy operator
		Client(const Client &lv) = delete;
		Client &operator = (const Client &lv) = delete;

		// read string from socket until \r\n pair has been readed
		std::string Read()
		{
			std::string result;
			// read while we can
			while (true)
			{
				ssize_t readed(0);
				if (_pos_begin == _pos_end)
				{
					// set read buffer start point and size to read
					int shift = std::distance(_buffer.begin(), _pos_begin);
					auto read_begin = _buffer.data() + shift;
					int size_to_read = _buffer.size() - shift;
					// do read data from socket
					readed = read(_sock.desc, read_begin, size_to_read);
					// break reading if needed
					if (readed <= 0)
						break;
					// reset iterators
					_pos_begin = _buffer.begin();
					_pos_end = _pos_begin + readed;
				}
				else
					readed = std::distance(_pos_begin, _pos_end);

				// search delim in buffer
				auto br = std::search(_pos_begin, _pos_end, _delim.begin(), _delim.end());
				if (br != _pos_end)
				{
					// append [begin, br) to result and break, if delim has been found
					result.append(_pos_begin, br);
					_pos_begin = br + _delim.size();
					break;
				}

				// check if \r\n pair has been splitted
				if (*std::prev(_pos_end) == *_delim.begin())
				{
					// (I hate \r\n pair) move \r into begin of buffer
					_buffer.front() = *std::prev(_pos_end);
					// append [begin, end - 1) to result and continue reading
					result.append(_pos_begin, std::prev(_pos_end));
					_pos_begin = _pos_end = std::next(_buffer.begin());
				}
				else
				{
					// append [begin, end) to result and continue reading
					result.append(_pos_begin, _pos_end);
					_pos_begin = _pos_end = _buffer.begin();
				}
			}

			return result;
		}

		// read data from socket with specified length
		std::vector<char> Read(const int bytes_to_read)
		{
			std::vector<char> result;
			result.reserve(bytes_to_read);

			// read while we can
			while (true)
			{
				ssize_t readed(0);
				if (_pos_begin == _pos_end)
				{
					// do read data from socket
					readed = read(_sock.desc, _buffer.data(), _buffer.size());
					// break reading if needed
					if (readed <= 0)
						break;
					// reset iterators
					_pos_begin = _buffer.begin();
					_pos_end = _pos_begin + readed;
				}
				else
					readed = std::distance(_pos_begin, _pos_end);

				// find last char to append into result
				auto br = (bytes_to_read - result.size() < readed)
					? _pos_begin + (bytes_to_read - result.size())
					: _pos_end;

				// append [begin, br) to result and continue reading
				std::copy(_pos_begin, br, std::back_inserter(result));
				// reset iterators
				_pos_end = _pos_begin = _buffer.begin();
				if (bytes_to_read == result.size())
					break;
			}

			return result;
		}

		bool Write(std::string str)
		{
			return true;
		}

		bool Write(const std::vector<char> &data)
		{
			return true;
		}
	private:
		// return static instance of command delimiter
		static const std::string &Delim()
		{
			static std::string delim("\r\n");
			return delim;
		}
	private:
		// client socket struct
		Sock _sock;
		// read buffer
		aBuffer _buffer;
		// delim between text commands
		std::string _delim;
		// begin of valid data in buffer
		aBuffer::iterator _pos_begin;
		// end of valid data in buffer
		aBuffer::iterator _pos_end;
	};
}