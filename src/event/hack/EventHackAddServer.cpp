#include "EventHackAddServer.hpp"


EventHackAddServer::EventHackAddServer(size_t userId,
                                       const std::string& name)
    : userId{userId}
    , name{name}
{
}

UUID EventHackAddServer::getEventUuid() const {
    return this->uuid;
}

size_t EventHackAddServer::getUserId() const {
    return userId;
}

std::string EventHackAddServer::getName() const {
    return name;
}
