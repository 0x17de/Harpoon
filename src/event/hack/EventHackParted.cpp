#include "EventHackParted.hpp"


UUID EventHackParted::getEventUuid() const {
    return this->uuid;
}

EventHackParted::EventHackParted(size_t userId,
                               size_t serverId,
                               const std::string& username,
                               const std::string& channel)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
{
}

size_t EventHackParted::getUserId() const {
    return userId;
}

size_t EventHackParted::getServerId() const {
    return serverId;
}

std::string EventHackParted::getUsername() const {
    return username;
}

std::string EventHackParted::getChannel() const {
    return channel;
}

