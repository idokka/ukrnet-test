#pragma once
#include <signal.h>
#include <functional>

namespace ukrnet
{
	// signals handler global static class
	class SigHandler
	{
	public:
		// signal handler functor
		typedef std::function<void(int)> onSignal;

	public:
		// get current SIGUSR1 handler
		static onSignal on_sigusr1() { return _on_sigusr1; }
		// set SIGUSR1 handler
		static void set_on_sigusr1(onSignal value) { SetEvent(SIGUSR1, value, _on_sigusr1); }

		// get current SIGUSR2 handler
		static onSignal on_sigusr2() { return _on_sigusr2; }
		// set SIGUSR2 handler
		static void set_on_sigusr2(onSignal value) { SetEvent(SIGUSR2, value, _on_sigusr2); }

	private:
		// static global signals handler
		static void OnSignal(int signum);
		// do set handler for specified signal
		static void SetEvent(int signum, onSignal value, onSignal &event);

	private:
		// SIGUSR1 handler
		static onSignal _on_sigusr1;
		// SIGUSR2 handler
		static onSignal _on_sigusr2;

	};

}