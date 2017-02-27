#include "EventHackReconnectServer.hpp"


EventHackReconnectServer::EventHackReconnectServer(size_t userId,
                                                   size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

UUID EventHackReconnectServer::getEventUuid() const {
    return this->uuid;
}

size_t EventHackReconnectServer::getUserId() const {
    return userId;
}

size_t EventHackReconnectServer::getServerId() const {
    return serverId;
}
