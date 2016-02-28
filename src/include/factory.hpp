#pragma once
#include <memory>
#include <functional>
#include "sock.hpp"

namespace ukrnet
{
	class Client;

	class IFactory
	{
	public:
		// custom client execution functor
		typedef std::function<void(Client &)> funcClientExecute;
		// custom client factory create functor
		typedef std::function<void(Sock, funcClientExecute)> funcClientCreate;

	public:
		// do create new client and execute it
		virtual void Create(Sock sock, funcClientExecute exec) = 0;

	public:
		// parse use conn exec model flags
		// thread model is default
		static std::shared_ptr<IFactory> ParseConnExecModel(bool use_thread, bool use_fork);

	};

	class FactoryThread : public IFactory
	{
	public:
		// do create new client and execute it
		void Create(Sock sock, funcClientExecute exec);

	private:
		// execute client functor
		void DoExec(Sock sock, funcClientExecute exec);

	};

	class FactoryFork : public IFactory
	{
	public:
		// do create new client and execute it
		void Create(Sock sock, funcClientExecute exec);
		
	};
}