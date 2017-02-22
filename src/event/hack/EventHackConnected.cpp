#include "EventHackConnected.hpp"


UUID EventHackConnected::getEventUuid() const {
    return this->uuid;
}

EventHackConnected::EventHackConnected(size_t userId, size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

size_t EventHackConnected::getUserId() const {
    return userId;
}

size_t EventHackConnected::getServerId() const {
    return serverId;
}

