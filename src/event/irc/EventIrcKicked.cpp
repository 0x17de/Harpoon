#include "EventIrcKicked.hpp"


UUID EventIrcKicked::getEventUuid() const {
    return this->uuid;
}

EventIrcKicked::EventIrcKicked(size_t userId,
                               size_t serverId,
                               const std::string& username,
                               const std::string& channel,
                               const std::string& target,
                               const std::string& reason)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
{
}

size_t EventIrcKicked::getUserId() const {
    return userId;
}

size_t EventIrcKicked::getServerId() const {
    return serverId;
}

std::string EventIrcKicked::getUsername() const {
    return username;
}

std::string EventIrcKicked::getChannel() const {
    return channel;
}

std::string EventIrcKicked::getTarget() const {
    return target;
}

std::string EventIrcKicked::getReason() const {
    return reason;
}

