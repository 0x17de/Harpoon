#include "EventIrcModeChanged.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcModeChanged::uuid = ::uuid.get();
UUID EventIrcModeChanged::getEventUuid() const {
    return this->uuid;
}

EventIrcModeChanged::EventIrcModeChanged(size_t userId, size_t serverId,
										 const std::string& username,
										 const std::string& channel,
										 const std::string& mode,
										 const std::string& arg)
	: userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
    , mode{mode}
    , arg{arg}
{
}

size_t EventIrcModeChanged::getUserId() const {
    return userId;
}

size_t EventIrcModeChanged::getServerId() const {
    return serverId;
}

std::string EventIrcModeChanged::getUsername() const {
    return username;
}

std::string EventIrcModeChanged::getChannel() const {
    return channel;
}

std::string EventIrcModeChanged::getMode() const {
    return mode;
}

std::string EventIrcModeChanged::getArg() const {
    return arg;
}

