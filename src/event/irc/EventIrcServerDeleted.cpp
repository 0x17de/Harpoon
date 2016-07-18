#include "EventIrcServerDeleted.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcServerDeleted::uuid = ::uuid.get();
UUID EventIrcServerDeleted::getEventUuid() const {
    return this->uuid;
}

EventIrcServerDeleted::EventIrcServerDeleted(size_t userId,
                                             size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

size_t EventIrcServerDeleted::getUserId() const {
    return userId;
}

size_t EventIrcServerDeleted::getServerId() const {
    return serverId;
}

