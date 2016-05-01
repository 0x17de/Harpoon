#include "IrcConnection.h"
#include "IrcConnection_Impl.h"
#include "event/EventQuit.h"


IrcConnection::IrcConnection(EventQueue* appQueue)
:
	EventLoop({
		EventQuit::uuid
	}),
	appQueue{appQueue}
{
}

IrcConnection::~IrcConnection() {
}

bool IrcConnection::onEvent(std::shared_ptr<IEvent> event) {
}

