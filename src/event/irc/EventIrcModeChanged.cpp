#include "EventIrcModeChanged.hpp"


UUID EventIrcModeChanged::getEventUuid() const {
    return this->uuid;
}

EventIrcModeChanged::EventIrcModeChanged(size_t userId, size_t serverId,
                                         const std::string& username,
                                         const std::string& channel,
                                         const std::string& mode,
                                         std::vector<std::string>::const_iterator argsBegin,
                                         std::vector<std::string>::const_iterator argsEnd)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
    , mode{mode}
    , args{argsBegin, argsEnd}
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

const std::vector<std::string>& EventIrcModeChanged::getArgs() const {
    return args;
}

