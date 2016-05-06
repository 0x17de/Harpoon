#include "EventIrcJoined.h"
#include "utils/uuidGen.h"


UUID EventIrcJoined::uuid = ::uuid.get();
UUID EventIrcJoined::getEventUuid() const {
	return this->uuid;
}

EventIrcJoined::EventIrcJoined(size_t serverId, std::string username, std::string channel)
:
	serverId{serverId},
	username{username},
	channel{channel}
{
}

size_t EventIrcJoined::getServerId() {
	return serverId;
}

std::string EventIrcJoined::getUsername() {
	return username;
}

std::string EventIrcJoined::getChannel() {
	return channel;
}

