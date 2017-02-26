#include "EventHackDeleteHost.hpp"


EventHackDeleteHost::EventHackDeleteHost(size_t userId,
                                         size_t serverId,
                                         const std::string& host,
                                         int port)
    : userId{userId}
    , serverId{serverId}
    , host{host}
    , port{port}
{
}

UUID EventHackDeleteHost::getEventUuid() const {
    return this->uuid;
}

size_t EventHackDeleteHost::getUserId() const {
    return userId;
}

size_t EventHackDeleteHost::getServerId() const {
    return serverId;
}

std::string EventHackDeleteHost::getHost() const {
    return host;
}

int EventHackDeleteHost::getPort() const {
    return port;
}
