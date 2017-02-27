#include "EventHackServerDeleted.hpp"


UUID EventHackServerDeleted::getEventUuid() const {
    return this->uuid;
}

EventHackServerDeleted::EventHackServerDeleted(size_t userId,
                                             size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

size_t EventHackServerDeleted::getUserId() const {
    return userId;
}

size_t EventHackServerDeleted::getServerId() const {
    return serverId;
}

