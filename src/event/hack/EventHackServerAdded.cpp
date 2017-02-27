#include "EventHackServerAdded.hpp"


UUID EventHackServerAdded::getEventUuid() const {
    return this->uuid;
}

EventHackServerAdded::EventHackServerAdded(size_t userId,
                                         size_t serverId,
                                         const std::string& serverName)
    : userId{userId}
    , serverId{serverId}
    , serverName{serverName}
{
}

size_t EventHackServerAdded::getUserId() const {
    return userId;
}

size_t EventHackServerAdded::getServerId() const {
    return serverId;
}

std::string EventHackServerAdded::getServerName() const {
    return serverName;
}

