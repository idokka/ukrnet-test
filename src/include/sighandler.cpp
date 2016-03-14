#include "sighandler.hpp"

using namespace ukrnet;

// SIGUSR1 handler
SigHandler::onSignal SigHandler::_on_sigusr1;
// SIGUSR2 handler
SigHandler::onSignal SigHandler::_on_sigusr2;

// get current SIGUSR1 handler
auto SigHandler::on_sigusr1() -> onSignal
{
	return _on_sigusr1;
}

// set SIGUSR1 handler
void SigHandler::set_on_sigusr1(onSignal value)
{
	SetEvent(SIGUSR1, value, _on_sigusr1);
}

// get current SIGUSR2 handler
auto SigHandler::on_sigusr2() -> onSignal
{
	return _on_sigusr2;
}

// set SIGUSR2 handler
void SigHandler::set_on_sigusr2(onSignal value)
{
	SetEvent(SIGUSR2, value, _on_sigusr2);
}

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
