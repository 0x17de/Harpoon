#include <iostream>
#include "ChatDatabaseDummy.h"
#include "event/EventQuit.h"
#include "event/EventLogin.h"
#include "event/EventLoginResult.h"

using namespace std;


ChatDatabaseDummy::ChatDatabaseDummy(EventQueue* appQueue) :
	EventLoop({
		EventQuit::uuid,
		EventLogin::uuid
	}),
	appQueue{appQueue}
{
}

void ChatDatabaseDummy::doLogin(EventLogin* login) {
	shared_ptr<EventLoginResult> loginResult{make_shared<EventLoginResult>()};
	//loginResult->client = login->client;
	loginResult->success = true;
	loginResult->userId = 1;
#warning login always successful
	appQueue->sendEvent(loginResult);
}

bool ChatDatabaseDummy::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventQuit::uuid) {
		std::cout << "DB received QUIT event" << std::endl;
		return false;
	} else if (eventType == EventLogin::uuid) {
		doLogin(dynamic_cast<EventLogin*>(event.get()));
	}
	return true;
}

