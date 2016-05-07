#include "User.hpp"
#include "irc/IrcConnection.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventQuit.hpp"
#include "event/EventActivateUser.hpp"
#include <iostream>

using namespace std;


User::User(EventQueue* appQueue)
:
	EventLoop({
		EventQuit::uuid,
		EventActivateUser::uuid
	}),
	appQueue{appQueue}
{
}

User::~User() {
}

bool User::onEvent(std::shared_ptr<IEvent> event) {
	UUID type = event->getEventUuid();
	if (type == EventQuit::uuid) {
		cout << "[US] received QUIT" << endl;
		for (auto& connection : ircConnections)
			connection.second.getEventQueue()->sendEvent(event);
		for (auto& connection : ircConnections)
			connection.second.join();
		return false;
	} else if (type == EventActivateUser::uuid) {
		cout << "[US] Received ActivateUser" << endl;
		auto activateEvent = event->as<EventActivateUser>();
		auto& loginConfiguration = activateEvent->getLoginConfiguration();
		for (auto entry : loginConfiguration) {
			auto& ircConfiguration = entry.second;
			ircConnections.emplace(piecewise_construct,
				forward_as_tuple(ircConfiguration.serverId),
				forward_as_tuple(appQueue, userId, ircConfiguration));
		}
	}
	return true;
}

