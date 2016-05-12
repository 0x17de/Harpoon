#include "EventIrcParted.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcParted::uuid = ::uuid.get();
UUID EventIrcParted::getEventUuid() const {
	return this->uuid;
}

EventIrcParted::EventIrcParted(size_t userId, size_t serverId, const std::string& username, const std::string& channel)
:
	userId{userId},
	serverId{serverId},
	username{username},
	channel{channel}
{
}

size_t EventIrcParted::getUserId() const {
	return userId;
}

size_t EventIrcParted::getServerId() const {
	return serverId;
}

std::string EventIrcParted::getUsername() const {
	return username;
}

std::string EventIrcParted::getChannel() const {
	return channel;
}

