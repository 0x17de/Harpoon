#include "EventHackDeleteServer.hpp"


EventHackDeleteServer::EventHackDeleteServer(size_t userId, size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

UUID EventHackDeleteServer::getEventUuid() const {
    return this->uuid;
}

size_t EventHackDeleteServer::getUserId() const {
    return userId;
}

size_t EventHackDeleteServer::getServerId() const {
    return serverId;
}
