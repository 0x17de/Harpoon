#include <iostream>
#include <csignal>
#include <list>

#include "Application.hpp"
#include "queue/EventQueue.hpp"
#include "user/UserManager.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "db/ChatDatabaseDummy.hpp"
#include "server/ws/WebsocketServer.hpp"

using namespace std;


Application::Application() :
	guard{this},
	EventLoop()
{
	EventQueue* queue = getEventQueue();
	userManager = make_shared<UserManager>(queue);
	eventHandlers.push_back(make_shared<ChatDatabaseDummy>(queue));
	//eventHandlers.push_back(make_shared<>());
	//server1 = make_shared<>(queue);
#ifdef USE_WEBSOCKET_SERVER
	eventHandlers.push_back(make_shared<WebsocketServer>(queue));
#endif

	for (auto& eventHandler : eventHandlers)
		eventHandler->getEventQueue()->sendEvent(make_shared<EventInit>());
}

bool Application::onEvent(std::shared_ptr<IEvent> event) {
	// which event do we process?
	UUID eventType = event->getEventUuid();

	#define TRANSMIT_EVENT(theQueue) \
		EventQueue* theQueue##Queue = theQueue->getEventQueue(); \
		if (theQueue##Queue->canProcessEvent(eventType)) \
			theQueue##Queue->sendEvent(event);
	TRANSMIT_EVENT(userManager)
	userManager->sendEventToUser(event); // sends only to matching user (only if IUserEvent)
	for (auto& eventHandler : eventHandlers) {
		TRANSMIT_EVENT(eventHandler)
	}
	#undef TRANSMIT_EVENT

	if (eventType == EventQuit::uuid) {
		cout << "Received Quit Event" << endl;
		userManager->join();
		for (auto& eventHandler : eventHandlers)
			eventHandler->join();
		//server1->join();
		cout << "Submodules were stopped" << endl;
		return false;
	}
	return true;
}

