#pragma once
#include <map>
#include <deque>
#include <mutex>
#include <limits>
#include <memory>
#include <sstream>
#include <algorithm>
#include <functional>

#include "include/server.hpp"
#include "include/client.hpp"
#include "include/logger.hpp"

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
			int hash;
			vecData data;
			int expire;
			int exp_at;

			// default constructor, fills zeros
			DataRec()
				: hash(0)
				, expire(0)
				, exp_at(0)
			{}

			// shrd ptr lesser by expire at
			static bool PtrLessByExpireAt(
				const std::shared_ptr<DataRec> &lv, 
				const std::shared_ptr<DataRec> &rv)
			{
				return lv.get()->exp_at < rv.get()->exp_at;
			}
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
		MemCached(int port)
			: _port(port)
		{
			// none of init
		}

		// do run server
		void Run()
		{
			// start server
			_server.set_port(_port);
			_server.set_client_func(GetClientFunc());
			_server.Open();
			if (_server.is_opened() == false)
			{
				logger().err("main", "failed to open server");
				return;
			}

			auto proc_exp = std::bind(& MemCached::ProcessExpireQueue, this);
			std::thread(proc_exp).detach();
			// start server listen
			_server.Accept();
		}

	private:
		// construct client func
		Server::funcClient GetClientFunc()
		{
			return std::bind(& MemCached::ClientFunc, this, std::placeholders::_1);
		}

		// client func
		void ClientFunc(Client &client)
		{
			Cmd cmd(Cmd::None);
			std::string key;

			while (client.CheckIsAlive())
			{
				std::stringstream ss(client.Read());
				cmd = ParseCmd(ss);
				if (cmd == Cmd::Set)
					DoSet(client, ss);
				else if (cmd == Cmd::Get)
					DoGet(client, ss);
				else if (cmd == Cmd::Del)
					DoDel(client, ss);
			}
		}

		// parse cmd from stream
		Cmd ParseCmd(std::istream &ss)
		{
			std::string cmd;
			ss >> cmd;
			if (cmd == "get")
				return Cmd::Get;
			else if (cmd == "set")
				return Cmd::Set;
			else if (cmd == "delete")
				return Cmd::Del;
			else
				return Cmd::None;
		}

		// process set command
		void DoSet(Client &client, std::istream &ss)
		{
			// read command details
			std::string key, value;
			int expires(0), data_len(-1);
			ss >> key >> value >> expires >> data_len;
			if (data_len < 0)
			{
				data_len = expires;
				expires = std::numeric_limits<int>::max();
			}
			logger().nfo("main", "set command recieved", key);

			{ // check and add to map
				mLock lock(_m_data);
				int hash = _str_hash(key);
				auto it = _data.find(hash);
				if (it != _data.end())
					logger().wrn("main", "key exists, do overwrite with new data", key);
				else
					it = _data.emplace(hash, std::make_shared<DataRec>()).first;
				auto rec_ptr = it->second;

				// fill data record
				rec_ptr->hash = hash;
				rec_ptr->expire = expires;
				rec_ptr->exp_at = expires + time(0);
				rec_ptr->data = client.Read(data_len);

				// add to expire queue
				auto hint = std::lower_bound(_expire_queue.begin(), _expire_queue.end(), 
					rec_ptr, DataRec::PtrLessByExpireAt);
				_expire_queue.insert(hint, rec_ptr);
			}
		}

		// process get command
		void DoGet(Client &client, std::istream &ss)
		{
			std::string key;
			ss >> key;
			logger().nfo("main", "get command recieved", key);
		}

		// process delete command
		void DoDel(Client &client, std::istream &ss)
		{
			std::string key;
			ss >> key;
			logger().nfo("main", "del command recieved", key);
		}

		// do process expire queue
		void ProcessExpireQueue()
		{
			while (true)
			{
				int now = (int)time(0);
				vecData queue_to_remove;

				{ // lock and process expire queue
					mLock lock(_m_data);
					while (_expire_queue.empty() == false)
					{
						auto rec_ptr = _expire_queue.front();
						if (rec_ptr->exp_at <= now)
						{
							_expire_queue.erase(_expire_queue.begin());
							_data.erase(rec_ptr->hash);
							queue_to_remove.push_back(rec_ptr);
						}
						else
							break;
					}
				}

				// log removed records
				for (auto rec_ptr : queue_to_remove)
				{
					logger().nfo("main", "element removed while it expires", rec_ptr->hash);
				}
				queue_to_remove.clear();

				// wait next iteration
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

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

	};
}