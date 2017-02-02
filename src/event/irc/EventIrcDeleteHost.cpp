#include "EventIrcDeleteHost.hpp"


EventIrcDeleteHost::EventIrcDeleteHost(size_t userId,
                                       size_t serverId,
                                       const std::string& host,
                                       int port)
    : userId{userId}
    , serverId{serverId}
    , host{host}
    , port{port}
{
}

UUID EventIrcDeleteHost::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcDeleteHost::getUserId() const {
    return userId;
}

size_t EventIrcDeleteHost::getServerId() const {
    return serverId;
}

std::string EventIrcDeleteHost::getHost() const {
    return host;
}

int EventIrcDeleteHost::getPort() const {
    return port;
}
