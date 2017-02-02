#include "EventIrcAddServer.hpp"


EventIrcAddServer::EventIrcAddServer(size_t userId,
                                     const std::string& name)
    : userId{userId}
    , name{name}
{
}

UUID EventIrcAddServer::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcAddServer::getUserId() const {
    return userId;
}

std::string EventIrcAddServer::getName() const {
    return name;
}
