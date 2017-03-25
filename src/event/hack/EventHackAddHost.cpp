#include "EventHackAddHost.hpp"


EventHackAddHost::EventHackAddHost(size_t userId,
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

UUID EventHackAddHost::getEventUuid() const {
    return this->uuid;
}

size_t EventHackAddHost::getUserId() const {
    return userId;
}

size_t EventHackAddHost::getServerId() const {
    return serverId;
}

std::string EventHackAddHost::getHost() const {
    return host;
}

std::string EventHackAddHost::getWebsocketUri() const {
    return websocketUri;
}

int EventHackAddHost::getPort() const {
    return port;
}

std::string EventHackAddHost::getPassword() const {
    return password;
}

bool EventHackAddHost::getIpV6() const {
    return ipV6;
}

bool EventHackAddHost::getSsl() const {
    return ssl;
}
