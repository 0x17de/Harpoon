#include "EventIrcServerAdded.hpp"


UUID EventIrcServerAdded::getEventUuid() const {
    return this->uuid;
}

EventIrcServerAdded::EventIrcServerAdded(size_t userId,
                                         size_t serverId,
                                         const std::string& serverName)
    : userId{userId}
    , serverId{serverId}
    , serverName{serverName}
{
}

size_t EventIrcServerAdded::getUserId() const {
    return userId;
}

size_t EventIrcServerAdded::getServerId() const {
    return serverId;
}

std::string EventIrcServerAdded::getServerName() const {
    return serverName;
}

