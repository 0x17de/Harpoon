#include "User.h"
#include "queue/EventQueue.h"
#include "event/EventQuit.h"


User::User(EventQueue* appQueue)
:
	EventLoop({
		EventQuit::uuid
	}),
	appQueue{appQueue}
{
}

User::~User() {
}

bool User::onEvent(std::shared_ptr<IEvent> event) {
}

