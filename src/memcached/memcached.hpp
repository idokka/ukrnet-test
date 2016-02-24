#pragma once
#include <map>
#include <deque>
#include <mutex>
#include <memory>

#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/sighandler.hpp"

namespace ukrnet
{
	// test MemCached server
	class MemCached
	{
	public:
		// server command
		enum class Cmd
		{
			None,
			Get,
			Set,
			Del
		};

		// one memcached record entity
		struct DataRec
		{
			// vector of char data
			typedef std::vector<char> vecData;
			// fields of data record
			std::string key;
			int hash;
			vecData data;
			int expire;
			int exp_at;

			// default constructor, fills zeros
			DataRec();
			// returns size of record data
			size_t size() const;
			// shrd ptr lesser by expire at asc, and then by hash asc
			static bool PtrLessByExpireAt(
				const std::shared_ptr<DataRec> &lv, 
				const std::shared_ptr<DataRec> &rv);
		};

		// string hash function
		typedef std::hash<std::string> strHash;
		// map data <int hash> = <data record>
		typedef std::map<int, std::shared_ptr<DataRec> > mapData;
		// deque of data records
		typedef std::deque<std::shared_ptr<DataRec>> deqData;
		// vector of data records
		typedef std::vector<std::shared_ptr<DataRec>> vecData;
		// lock guard for mutex
		typedef std::lock_guard<std::mutex> mLock;

	public:
		// default constructor
		// port: server socket port
		MemCached(int port);
		// do run server
		void Run();

		// construct signal user handler function
		SigHandler::onSignal GetSigUsr1Handler();
		// construct signal user handler function
		SigHandler::onSignal GetSigUsr2Handler();

	private:
		// construct client func
		Server::funcClient GetClientFunc();
		// SIGUSR1 handler: save data to file
		void SignalUser1Handler(int signum);
		// SIGUSR2 handler: save data to file
		void SignalUser2Handler(int signum);

		// client func
		void ClientFunc(Client &client);

		// parse cmd from stream
		Cmd ParseCmd(std::istream &ss);
		// process set command
		void DoSet(Client &client, std::istream &ss);
		// process get command
		void DoGet(Client &client, std::istream &ss);
		// process delete command
		void DoDel(Client &client, std::istream &ss);

		// do process expire queue
		void ProcessExpireQueue();
		// update data inf mmap file
		void UpdateMapFile(vecData &data);

	private:
		// server socket port
		int _port;
		// server socket
		Server _server;
		// data map
		mapData _data;
		// data expire deque
		deqData _expire_queue;
		// mutex for sync access to data map
		std::mutex _m_data;
		// string hasher
		strHash _str_hash;
		// mutex for sync access to data file
		std::mutex _m_data_file;

	private:
		static const std::string _data_file_path;

	};
}