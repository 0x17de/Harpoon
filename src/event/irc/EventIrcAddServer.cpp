#include "EventIrcAddServer.hpp"
#include "utils/uuidGen.hpp"


EventIrcAddServer::EventIrcAddServer(size_t userId,
                                     std::string name)
    : userId{userId}
    , name{name}
{
}

UUID EventIrcAddServer::uuid = ::uuid.get();
UUID EventIrcAddServer::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcAddServer::getUserId() const {
    return userId;
}

std::string EventIrcAddServer::getName() const {
    return name;
}
