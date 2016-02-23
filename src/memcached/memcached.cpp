#include "memcached.hpp"

using namespace ukrnet;


// default constructor, fills zeros
MemCached::DataRec::DataRec()
	: hash(0)
	, expire(0)
	, exp_at(0)
{}

// shrd ptr lesser by expire at asc, and then by hash asc
static bool MemCached::DataRec::PtrLessByExpireAt(
	const std::shared_ptr<DataRec> &lv, 
	const std::shared_ptr<DataRec> &rv)
{
	if (lv.get()->exp_at != rv.get()->exp_at)
		return lv.get()->exp_at < rv.get()->exp_at;
	else
		lv.get()->hash < rv.get()->hash;
}

// default constructor
// port: server socket port
MemCached::MemCached(int port)
	: _port(port)
{
	// none of init
}

// do run server
void MemCached::Run()
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

// construct signal user handler function
SigHandler::onSignal MemCached::GetSigUsr1Handler()
{
	return std::bind(& MemCached::SignalUser1Handler, this, std::placeholders::_1);
}

// construct signal user handler function
SigHandler::onSignal MemCached::GetSigUsr2Handler()
{
	return std::bind(& MemCached::SignalUser2Handler, this, std::placeholders::_1);
}

// construct client func
Server::funcClient MemCached::GetClientFunc()
{
	return std::bind(& MemCached::ClientFunc, this, std::placeholders::_1);
}

// SIGUSR1 handler: save data to file
void MemCached::SignalUser1Handler(int signum)
{
	logger().nfo("main", "SIGUSR1 catched");
	std::ofstream fs("/tmp/memcached");

	{ // lock data arrays
		mLock lock(_m_data);
		for (auto rec_ptr : _expire_queue)
		{
			std::string value(rec_ptr->data.begin(), rec_ptr->data.end());
			fs << '\'' << rec_ptr->key << "\' -> \'" << value << '\'' << std::endl;
			// TODO: except case if data contains special characters
		}
	}

	fs.flush();
	fs.close();
}

// SIGUSR2 handler: save data to file
void MemCached::SignalUser2Handler(int signum)
{
	logger().nfo("main", "SIGUSR2 catched");
	std::ofstream fs("/tmp/memcached");

	{ // lock data arrays
		mLock lock(_m_data);
		for (auto rec_ptr : _expire_queue)
		{
			std::string value(rec_ptr->data.begin(), rec_ptr->data.end());
			fs 
				<< rec_ptr->key << tab
				<< rec_ptr->hash << tab
				<< rec_ptr->expire << tab
				<< rec_ptr->exp_at << tab
				<< value.size() << tab
				<< value << std::endl;
			// TODO: except case if data contains special characters
		}
	}

	fs.flush();
	fs.close();
}

// client func
void MemCached::ClientFunc(Client &client)
{
	Cmd cmd(Cmd::None);
	std::string key;

	//while (client.CheckIsAlive()) 
	// TODO: wrong implement, os error 11 - resource temp unavailable
	while (true)
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
auto MemCached::ParseCmd(std::istream &ss) -> Cmd
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
void MemCached::DoSet(Client &client, std::istream &ss)
{
	// read command details
	std::string key;
	int expires(0), data_len(-1);
	ss >> key >> expires >> data_len;
	if (data_len < 0)
	{
		data_len = expires;
		expires = 0;
	}
	auto data_value = client.Read(data_len);
	logger().nfo("main", "set command recieved", key);

	{ // check and add to map
		mLock lock(_m_data);
		int hash = _str_hash(key);
		auto it = _data.find(hash);
		if (it != _data.end())
		{
			logger().wrn("main", "key exists, do overwrite with new data", key);
			client.Write("EXISTS");
			client.WriteEndl();
		}
		else
		{
			std::string value(data_value.begin(), data_value.end());
			logger().nfo("main", "data stored", key, value);
			it = _data.emplace(hash, std::make_shared<DataRec>()).first;
			client.Write("STORED");
			client.WriteEndl();
		}
		auto rec_ptr = it->second;

		// fill data record
		rec_ptr->key = key;
		rec_ptr->hash = hash;
		rec_ptr->expire = expires;
		rec_ptr->exp_at = (expires == 0) ? std::numeric_limits<int>::max() : (expires + time(0));
		rec_ptr->data = data_value;

		// add to expire queue
		auto hint = std::lower_bound(_expire_queue.begin(), _expire_queue.end(), 
			rec_ptr, DataRec::PtrLessByExpireAt);
		_expire_queue.insert(hint, rec_ptr);
		// TODO: remove old record from expire queue if element was exists
	}
}

// process get command
void MemCached::DoGet(Client &client, std::istream &ss)
{
	std::string key;
	ss >> key;
	logger().nfo("main", "get command recieved", key);
	std::shared_ptr<DataRec> rec_ptr;

	{ // check and add to map
		mLock lock(_m_data);
		int hash = _str_hash(key);
		auto it = _data.find(hash);
		if (it != _data.end())
			rec_ptr = it->second;
	}

	// do answer to client
	if (rec_ptr)
	{
		std::stringstream os;
		// formatting output string and send it
		os << "VALUE " << rec_ptr->key << " " << rec_ptr->data.size();
		client.Write(os.str());
		client.WriteEndl();
		client.Write(rec_ptr->data);
		client.WriteEndl();
	}
	else
	{
		logger().wrn("main", "key not exists", key);
	}
	client.Write("END");
	client.WriteEndl();
}

// process delete command
void MemCached::DoDel(Client &client, std::istream &ss)
{
	std::string key;
	ss >> key;
	logger().nfo("main", "del command recieved", key);
	std::shared_ptr<DataRec> rec_ptr;

	{ // check and add to map
		mLock lock(_m_data);
		int hash = _str_hash(key);
		auto it = _data.find(hash);
		if (it != _data.end())
		{
			rec_ptr = it->second;
			_data.erase(it);
			auto range = std::equal_range(_expire_queue.begin(), _expire_queue.end(), 
				rec_ptr, DataRec::PtrLessByExpireAt);
			auto exp_it = std::find(range.first, range.second, rec_ptr);
			if (exp_it != range.second)
			{
				_expire_queue.erase(exp_it);
			}
		}
	}

	// do answer to client
	if (rec_ptr)
	{
		client.Write("DELETED");
		client.WriteEndl();
		logger().nfo("main", "element deleted", key);
	}
	else
	{
		client.Write("NOT_FOUND");
		client.WriteEndl();
		logger().wrn("main", "key not exists", key);
	}
}

// do process expire queue
void MemCached::ProcessExpireQueue()
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
			logger().nfo("main", "element removed while it expires", rec_ptr->key);
		}
		queue_to_remove.clear();

		// wait next iteration
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
