#include "EventIrcAction.hpp"


UUID EventIrcAction::getEventUuid() const {
    return this->uuid;
}

EventIrcAction::EventIrcAction(size_t userId,
                               size_t serverId,
                               const std::string& username,
                               const std::string& channel,
                               const std::string& message)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
    , message{message}
{
}

size_t EventIrcAction::getUserId() const {
    return userId;
}

size_t EventIrcAction::getServerId() const {
    return serverId;
}

std::string EventIrcAction::getUsername() const {
    return username;
}

std::string EventIrcAction::getChannel() const {
    return channel;
}

std::string EventIrcAction::getMessage() const {
    return message;
}
