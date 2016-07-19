#include "EventIrcAddHost.hpp"
#include "utils/uuidGen.hpp"


EventIrcAddHost::EventIrcAddHost(size_t userId,
                                 size_t serverId,
                                 const std::string& host,
                                 int port,
                                 const std::string& password,
                                 bool ipV6,
                                 bool ssl)
    : userId{userId}
    , serverId{serverId}
    , host{host}
    , password{password}
    , port{port}
    , ipV6{ipV6}
    , ssl{ssl}
{
}

UUID EventIrcAddHost::uuid = ::uuid.get();
UUID EventIrcAddHost::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcAddHost::getUserId() const {
    return userId;
}

size_t EventIrcAddHost::getServerId() const {
    return serverId;
}

std::string EventIrcAddHost::getHost() const {
    return host;
}

std::string EventIrcAddHost::getPassword() const {
    return password;
}

int EventIrcAddHost::getPort() const {
    return port;
}

bool EventIrcAddHost::getIpV6() const {
    return ipV6;
}

bool EventIrcAddHost::getSsl() const {
    return ssl;
}
