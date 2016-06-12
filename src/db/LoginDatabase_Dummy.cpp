#include <iostream>
#include "LoginDatabase_Dummy.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLoginResult.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"

using namespace std;


LoginDatabase_Dummy::LoginDatabase_Dummy(EventQueue* appQueue) :
	EventLoop({
		EventInit::uuid,
		EventQuit::uuid,
		EventLogin::uuid
	}),
	appQueue{appQueue}
{
}

bool LoginDatabase_Dummy::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventQuit::uuid) {
		std::cout << "DB received QUIT event" << std::endl;
		return false;
	} else if (eventType == EventLogin::uuid) {
		auto login = event->as<EventLogin>();
		bool success = login->getUsername() == "user" && login->getPassword() == "password";
		shared_ptr<EventLoginResult> loginResult{make_shared<EventLoginResult>(login->getTarget(), success, 1, login->getData())};
		appQueue->sendEvent(loginResult);
	}
	return true;
}

