#include "EventIrcSettingsListing.hpp"
#include "service/irc/IrcChannelLoginData.hpp"
#include "service/irc/IrcServerHostConfiguration.hpp"
#include "service/irc/IrcServerConfiguration.hpp"


UUID EventIrcSettingsListing::getEventUuid() const {
    return this->uuid;
}

EventIrcSettingsListing::EventIrcSettingsListing(size_t userId, void* data)
    : userId{userId}
    , data{data}
{
}

IrcServerConfiguration& EventIrcSettingsListing::addServer(size_t serverId, std::string serverName) {
    servers.emplace_back(serverId, serverName);
    return servers.back();
}

size_t EventIrcSettingsListing::getUserId() const {
    return userId;
}

const std::list<IrcServerConfiguration>& EventIrcSettingsListing::getServerList() const {
    return servers;
}

void* EventIrcSettingsListing::getData() const {
    return data;
}

