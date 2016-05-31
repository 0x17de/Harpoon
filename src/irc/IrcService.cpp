#include "IrcService.hpp"
#include "irc/IrcConnection.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventQuit.hpp"
#include "event/irc/EventIrcActivateUser.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include <iostream>

using namespace std;


IrcService::IrcService(size_t userId, EventQueue* appQueue)
:
	EventLoop({
		EventQuit::uuid,
		EventIrcJoinChannel::uuid,
		EventIrcSendMessage::uuid
	}, {
		&EventGuard<IActivateUserEvent>
	}),
	userId{userId},
	appQueue{appQueue}
{
}

IrcService::~IrcService() {
}

bool IrcService::onEvent(std::shared_ptr<IEvent> event) {
	UUID type = event->getEventUuid();

	auto activateUser = event->as<EventIrcActivateUser>();
	if (activateUser) {
		cout << "[US] Received ActivateUser" << endl;
		auto& loginConfiguration = activateUser->getLoginConfiguration();
		for (auto entry : loginConfiguration) {
			auto& ircConfiguration = entry.second;
			cout << "[US] CONFIG: " << ircConfiguration.serverId << endl;
			ircConnections.emplace(piecewise_construct,
				forward_as_tuple(ircConfiguration.serverId),
				forward_as_tuple(appQueue, userId, ircConfiguration));
		}
	}

	if (type == EventQuit::uuid) {
		cout << "[US] received QUIT" << endl;
		for (auto& connection : ircConnections)
			connection.second.getEventQueue()->sendEvent(event);
		for (auto& connection : ircConnections)
			connection.second.join();
		return false;
	} else if (type == EventIrcJoinChannel::uuid) {
		cout << "[US] Received JOIN" << endl;
	} else if (type == EventIrcSendMessage::uuid) {
		auto message = event->as<EventIrcSendMessage>();
		auto it = ircConnections.find(message->getServerId());
		if (it != ircConnections.end())
			it->second.getEventQueue()->sendEvent(event);
	}
	return true;
}

