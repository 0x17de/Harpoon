#include "EventIrcJoined.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcJoined::uuid = ::uuid.get();
UUID EventIrcJoined::getEventUuid() const {
	return this->uuid;
}

EventIrcJoined::EventIrcJoined(size_t userId, size_t serverId, const std::string& username, const std::string& channel)
:
	userId{userId},
	serverId{serverId},
	username{username},
	channel{channel}
{
}

size_t EventIrcJoined::getUserId() const {
	return userId;
}

size_t EventIrcJoined::getServerId() const {
	return serverId;
}

std::string EventIrcJoined::getUsername() const {
	return username;
}

std::string EventIrcJoined::getChannel() const {
	return channel;
}

