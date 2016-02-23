#pragma once
#include <array>
#include <vector>
#include "sock.hpp"

namespace ukrnet
{
	// socket client stream
	class Client
	{
	public:
		typedef std::array<char, 255> aBuffer;
	public:
		// default constructor
		// sock: Sock structure for client socket
		Client(Sock sock);
		// destructor; close socket, if needed
		~Client();
		// disable copy constructor
		Client(const Client &lv) = delete;
		// disable copy operator
		Client &operator = (const Client &lv) = delete;

		// read string from socket until \r\n pair has been readed
		std::string Read();
		// read data from socket with specified length
		std::vector<char> Read(const int bytes_to_read);

		// write string to client socket
		bool Write(std::string str);
		// write data to client socket
		bool Write(const std::vector<char> &data);
		// write endl \r\n to socket
		bool WriteEndl();

		// check is client socket alive
		bool CheckIsAlive();

	public:
		// returns is client socket opened 
		bool is_opened() const { return _is_opened; }

	private:
		// is socket opened
		bool _is_opened;
		// client socket struct
		Sock _sock;
		// read buffer
		aBuffer _buffer;
		// delim between text commands
		static const std::string _delim;
		// begin of valid data in buffer
		aBuffer::iterator _pos_begin;
		// end of valid data in buffer
		aBuffer::iterator _pos_end;

	};
}