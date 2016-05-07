#include <iostream>
#include "UserManager.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"

using namespace std;


UserManager::UserManager(EventQueue* appQueue) :
	EventLoop({
		EventQuit::uuid,
		EventLoginResult::uuid
	}),
	appQueue{appQueue}
{
}

void UserManager::doLogin(EventLoginResult* login) {
	cout << "Login:" << endl
	<< "Success: " << login->success << endl
	<< "UserID: " << login->userId << endl; 
	if (login->success) {
#warning doLogin for user stub
	}
}

bool UserManager::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();

	if (eventType == EventQuit::uuid) {
		cout << "UserManager received QUIT event" << endl;
		return false;
	} else if (eventType == EventLoginResult::uuid) {
		doLogin(dynamic_cast<EventLoginResult*>(event.get()));
	}
	return true;
}
