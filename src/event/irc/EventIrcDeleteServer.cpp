#include "EventIrcDeleteServer.hpp"
#include "utils/uuidGen.hpp"


EventIrcDeleteServer::EventIrcDeleteServer(size_t userId, size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

UUID EventIrcDeleteServer::uuid = ::uuid.get();
UUID EventIrcDeleteServer::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcDeleteServer::getUserId() const {
    return userId;
}

size_t EventIrcDeleteServer::getServerId() const {
    return serverId;
}
