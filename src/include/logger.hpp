#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include "stream-helper.hpp"

namespace ukrnet
{
	// logger class
	// can write log to console and specified file
	class Logger
	{
	public:
		typedef std::lock_guard<std::mutex> mlock;
	public:
		// default constructor
		Logger()
			: _write_to_console(true)
			, _write_to_file(true)
		{
			// none of init
		}

		// init logger: try to open log file
		// returns true, is successful, or false otherwise
		bool init()
		{
			if (_log_path.empty())
				return false;
			_log_file.open(_log_path);
			if (_log_file.is_open() == false)
				return false;

			return true;
		}

		template <typename ... Values>
		void nfo(std::string src, Values ... values)
		{
			log("[nfo]", src, values...);
		}
		template <typename ... Values>
		void err(std::string src, Values ... values)
		{
			log("[err]", src, values...);
		}
		template <typename ... Values>
		void wrn(std::string src, Values ... values)
		{
			log("[wrn]", src, values...);
		}

	public:
		// get path to log file
		const std::string &log_path() const { return _log_path; }
		// set path to log file
		void set_log_path(const std::string &log_path) { _log_path = log_path; }
		// get write to console state
		bool write_to_console() const { return _write_to_console; }
		// set write to console state
		void set_write_to_console(bool value) { _write_to_console = value; }
		// get write to file state
		bool write_to_file() const { return _write_to_file; }
		// set write to file state
		void set_write_to_file(bool value) { _write_to_file = value; }
		
	private:
		template <typename ... Values>
		void log(std::string level, std::string src, Values ... values)
		{
			mlock lock(_m_do_log);
			if (_log_file.is_open() && _write_to_file)
				_log_file << date_time << tab << level << tab << "[" << src << "]" << tab;
			if (_write_to_console)
				std::cout << date_time << tab << level << tab << "[" << src << "]" << tab;
			log_next(values...);
		}

		template <typename Value, typename ... Values>
		void log_next(Value value, Values ... values)
		{
			if (_log_file.is_open() && _write_to_file)
				_log_file << value << tab;
			if (_write_to_console)
				std::cout << value << tab;
			log_next(values...);
		}

		template <typename Value>
		void log_next(Value value)
		{
			if (_log_file.is_open() && _write_to_file)
				_log_file << value << std::endl;
			if (_write_to_console)
				std::cout << value << std::endl;
		}
	private:
		// path to log file
		std::string _log_path;
		// log file stream
		std::ofstream _log_file;
		// write log to console
		bool _write_to_console;
		// write log to file
		bool _write_to_file;
		// mutex for sync access to log file and console
		std::mutex _m_do_log;
	};

	// returns static logger instance
	inline Logger &logger()
	{
		static Logger logger;
		return logger;
	}
}