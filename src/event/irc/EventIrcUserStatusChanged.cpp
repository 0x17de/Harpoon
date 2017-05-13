#include "EventIrcUserStatusChanged.hpp"


UUID EventIrcUserStatusChanged::getEventUuid() const {
    return this->uuid;
}

EventIrcUserStatusChanged::EventIrcUserStatusChanged(size_t userId,
                                                     size_t serverId,
                                                     Status status,
                                                     const std::string& username,
                                                     const std::string& channel,
                                                     const std::string& targetOrMode,
                                                     const std::string& reason)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
    , status{status}
    , targetOrMode{targetOrMode}
    , reason{reason}
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

std::string EventIrcUserStatusChanged::getTarget() const {
    return targetOrMode;
}

std::string EventIrcUserStatusChanged::getMode() const {
    return targetOrMode;
}

std::string EventIrcUserStatusChanged::getReason() const {
    return reason;
}
