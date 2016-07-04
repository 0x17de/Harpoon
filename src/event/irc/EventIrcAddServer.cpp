#include "EventIrcAddServer.hpp"
#include "utils/uuidGen.hpp"
#include "service/irc/IrcChannelLoginData.hpp"


EventIrcAddServer::EventIrcAddServer(size_t userId, std::string serverName)
    : userId{userId}
    , serverName{serverName}
{
}

UUID EventIrcAddServer::uuid = ::uuid.get();
UUID EventIrcAddServer::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcAddServer::getUserId() const {
    return userId;
}

std::string EventIrcAddServer::getServerName() const {
    return serverName;
}
