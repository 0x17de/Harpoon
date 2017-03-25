#include "EventHackHostAdded.hpp"


UUID EventHackHostAdded::getEventUuid() const {
    return this->uuid;
}

EventHackHostAdded::EventHackHostAdded(size_t userId,
                                       size_t serverId,
                                       const std::string& host,
                                       const std::string& websocketUri,
                                       int port,
                                       const std::string& password,
                                       bool ipV6,
                                       bool ssl)
    : userId{userId}
    , serverId{serverId}
    , host{host}
    , websocketUri{websocketUri}
    , port{port}
    , password{password}
    , ipV6{ipV6}
    , ssl{ssl}
{
}

size_t EventHackHostAdded::getUserId() const {
    return userId;
}

size_t EventHackHostAdded::getServerId() const {
    return serverId;
}

std::string EventHackHostAdded::getHost() const {
    return host;
}

std::string EventHackHostAdded::getWebsocketUri() const {
    return websocketUri;
}

int EventHackHostAdded::getPort() const {
    return port;
}

std::string EventHackHostAdded::getPassword() const {
    return password;
}

bool EventHackHostAdded::getIpV6() const {
    return ipV6;
}

bool EventHackHostAdded::getSsl() const {
    return ssl;
}
