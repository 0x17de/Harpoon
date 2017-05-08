#include "EventIrcUserStatusChanged.hpp"


UUID EventIrcUserStatusChanged::getEventUuid() const {
    return this->uuid;
}

EventIrcUserStatusChanged::EventIrcUserStatusChanged(size_t userId,
                                       size_t serverId,
                                       const std::string& username,
                                       const std::string& channel,
                                       Status status)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
    , status{status}
{
}

size_t EventIrcUserStatusChanged::getUserId() const {
    return userId;
}

size_t EventIrcUserStatusChanged::getServerId() const {
    return serverId;
}

std::string EventIrcUserStatusChanged::getUsername() const {
    return username;
}

std::string EventIrcUserStatusChanged::getChannel() const {
    return channel;
}

EventIrcUserStatusChanged::Status EventIrcUserStatusChanged::getStatus() const {
    return status;
}
