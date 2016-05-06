#include "EventIrcParted.h"
#include "utils/uuidGen.h"


UUID EventIrcParted::uuid = ::uuid.get();
UUID EventIrcParted::getEventUuid() const {
	return this->uuid;
}

EventIrcParted::EventIrcParted(size_t serverId, std::string username, std::string channel)
:
	serverId{serverId},
	username{username},
	channel{channel}
{
}

size_t EventIrcParted::getServerId() {
	return serverId;
}

std::string EventIrcParted::getUsername() {
	return username;
}

std::string EventIrcParted::getChannel() {
	return channel;
}

