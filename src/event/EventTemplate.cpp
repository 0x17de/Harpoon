#include "EventTemplate.hpp"


UUID EventTemplate::getEventUuid() const {
    return this->uuid;
}

EventTemplate::EventTemplate(size_t userId, size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

size_t EventTemplate::getUserId() const {
    return userId;
}

size_t EventTemplate::getServerId() const {
    return serverId;
}

