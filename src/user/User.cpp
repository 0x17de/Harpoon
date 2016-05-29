#include "User.hpp"
#include "irc/IrcConnection.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventQuit.hpp"
#include "event/EventActivateUser.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include <iostream>

using namespace std;


User::User(size_t userId, EventQueue* appQueue)
:
	EventLoop({
		EventQuit::uuid,
		EventActivateUser::uuid,
		EventIrcJoinChannel::uuid,
		EventIrcSendMessage::uuid
	}),
	userId{userId},
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
	} else if (type == EventIrcJoinChannel::uuid) {
		cout << "[US] Received JOIN" << endl;
	} else if (type == EventActivateUser::uuid) {
		cout << "[US] Received ActivateUser" << endl;
		auto activateEvent = event->as<EventActivateUser>();
		auto& loginConfiguration = activateEvent->getLoginConfiguration();
		for (auto entry : loginConfiguration) {
			auto& ircConfiguration = entry.second;
			cout << "[US] CONFIG: " << ircConfiguration.serverId << endl;
			ircConnections.emplace(piecewise_construct,
				forward_as_tuple(ircConfiguration.serverId),
				forward_as_tuple(appQueue, userId, ircConfiguration));
		}
	} else if (type == EventIrcSendMessage::uuid) {
		auto message = event->as<EventIrcSendMessage>();
		auto it = ircConnections.find(message->getServerId());
		if (it != ircConnections.end())
			it->second.getEventQueue()->sendEvent(event);
	}
	return true;
}

