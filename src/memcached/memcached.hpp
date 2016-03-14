#pragma once
#include <map>
#include <deque>
#include <mutex>
#include <memory>

#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/factory.hpp"
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
			// key hash
			int hash;
			// key data
			std::string key;
			// stored value
			vecData data;
			// expire in value, secs
			int expire;
			// expire at value, secs
			int exp_at;

			// default constructor, fills zeros
			DataRec();
			// key constructor
			DataRec(std::string key);
			// returns size of record data
			size_t size() const;

			// shrd ptr lesser by expire at asc, and then by hash asc
			static bool PtrLessByExpireAt(
				const std::shared_ptr<DataRec> &lv, 
				const std::shared_ptr<DataRec> &rv);

			// shrd ptr lesser by key hash asc, and then by key data asc
			struct PtrLessByHash
			{
				bool operator()(
					const std::shared_ptr<DataRec> &lv, 
					const std::shared_ptr<DataRec> &rv);
			};
		};

		// string hash function
		typedef std::hash<std::string> strHash;
		// map data <data record as key> = <data record as value>
		typedef std::map<
			std::shared_ptr<DataRec>, 
			std::shared_ptr<DataRec>,
			DataRec::PtrLessByHash> mapData;
		// deque of data records
		typedef std::deque<std::shared_ptr<DataRec>> deqData;
		// vector of data records
		typedef std::vector<std::shared_ptr<DataRec>> vecData;
		// lock guard for mutex
		typedef std::lock_guard<std::mutex> mLock;

	public:
		// default constructor
		// port: server socket port
		// use_thread: use thread connection execution model
		// use_fork: use fork connection execution model
		MemCached(int port, bool use_thread, bool use_fork);
		// do run server
		void Run();

		// construct signal user handler function
		SigHandler::onSignal GetSigUsr1Handler();
		// construct signal user handler function
		SigHandler::onSignal GetSigUsr2Handler();

	private:
		// construct client func
		IFactory::funcClientExecute GetClientFunc();
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
		// mutex for sync access to data file
		std::mutex _m_data_file;
		// connection execute factory
		std::shared_ptr<IFactory> _factory;

	private:
		// path to data file
		static const std::string _data_file_path;

	};
}