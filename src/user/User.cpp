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
	UUID type = event->getEventUuid();
	if (type == EventQuit::uuid) {
		return false;
	}
	return true;
}

