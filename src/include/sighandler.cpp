#include "sighandler.hpp"

using namespace ukrnet;

// SIGUSR1 handler
SigHandler::onSignal SigHandler::_on_sigusr1;
// SIGUSR2 handler
SigHandler::onSignal SigHandler::_on_sigusr2;

// static global signals handler
void SigHandler::OnSignal(int signum)
{
	if ((signum == SIGUSR1) && _on_sigusr1)
		_on_sigusr1(signum);
	else if ((signum == SIGUSR2) && _on_sigusr2)
		_on_sigusr2(signum);
}

// do set handler for specified signal
void SigHandler::SetEvent(int signum, onSignal value, onSignal &event)
{
	event = value;
	if (event)
		signal(signum, OnSignal);
	else
		signal(signum, SIG_DFL);
}
