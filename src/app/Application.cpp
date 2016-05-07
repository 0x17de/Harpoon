#include <iostream>
#include <csignal>
#include <list>

#include "Application.hpp"
#include "queue/EventQueue.hpp"
#include "user/UserManager.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "db/ChatDatabaseDummy.hpp"

using namespace std;


Application::Application() :
	guard{this},
	EventLoop()
{
	EventQueue* queue = getEventQueue();
	userManager = make_shared<UserManager>(queue);
	database = make_shared<ChatDatabaseDummy>(queue);
	//server1 = make_shared<>(queue);

	database->getEventQueue()->sendEvent(make_shared<EventInit>());
}

bool Application::onEvent(std::shared_ptr<IEvent> event) {
	// which event do we process?
	UUID eventType = event->getEventUuid();

	#define TRANSMIT_EVENT(theQueue) \
		EventQueue* theQueue##Queue = theQueue->getEventQueue(); \
		if (theQueue##Queue->canProcessEvent(eventType)) \
			theQueue##Queue->sendEvent(event);
	TRANSMIT_EVENT(userManager)
	TRANSMIT_EVENT(database)
	//TRANSMIT_EVENT(server1)
	#undef TRANSMIT_EVENT

	if (eventType == EventQuit::uuid) {
		cout << "Received Quit Event" << endl;
		database->join();
		userManager->join();
		//server1->join();
		cout << "Submodules were stopped" << endl;
		return false;
	}
	return true;
}

