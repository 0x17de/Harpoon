#include "IrcConnection.h"
#include "IrcConnection_Impl.h"
#include "event/EventQuit.h"
#include "event/EventIrcJoinChannel.h"
#include <map>

using namespace std;


static map<irc_session_t*, IrcConnection_Impl*> activeIrcConnections;


template <void (IrcConnection_Impl::*F)(irc_session_t*, const char*, const char*, const char**, unsigned int)>
void onIrcEvent(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
	IrcConnection_Impl* cxn = activeIrcConnections.at(session);
	(cxn->*F)(session, event, origin, params, count);
}


void IrcConnection_Impl::onConnect(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
}


IrcConnection::IrcConnection(EventQueue* appQueue)
:
	impl{new IrcConnection_Impl()},
	EventLoop({
		EventQuit::uuid,
		EventIrcJoinChannel::uuid
	}),
	appQueue{appQueue}
{
	irc_callbacks_t callbacks;
	callbacks.event_connect = &onIrcEvent<&IrcConnection_Impl::onConnect>;
	impl->ircSession = irc_create_session(&callbacks);
	if (impl->ircSession != 0) {
		activeIrcConnections.emplace(impl->ircSession, impl.get());
	}
}

IrcConnection::~IrcConnection() {
	if (impl->ircSession != 0) {
		activeIrcConnections.erase(impl->ircSession);
		irc_destroy_session(impl->ircSession);
	}
}

bool IrcConnection::onEvent(std::shared_ptr<IEvent> event) {
	UUID type = event->getEventUuid();
	if (type == EventQuit::uuid) {
		return false;
	}
	return true;
}

