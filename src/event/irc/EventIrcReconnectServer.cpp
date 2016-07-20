#include "EventIrcReconnectServer.hpp"
#include "utils/uuidGen.hpp"


EventIrcReconnectServer::EventIrcReconnectServer(size_t userId,
                                                 size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

UUID EventIrcReconnectServer::uuid = ::uuid.get();
UUID EventIrcReconnectServer::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcReconnectServer::getUserId() const {
    return userId;
}

size_t EventIrcReconnectServer::getServerId() const {
    return serverId;
}
