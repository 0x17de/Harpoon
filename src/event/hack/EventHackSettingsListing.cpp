#include "EventHackSettingsListing.hpp"
#include "service/irc/HackChannelLoginData.hpp"
#include "service/irc/HackServerHostConfiguration.hpp"
#include "service/irc/HackServerConfiguration.hpp"


UUID EventHackSettingsListing::getEventUuid() const {
    return this->uuid;
}

EventHackSettingsListing::EventHackSettingsListing(size_t userId, void* data)
    : userId{userId}
    , data{data}
{
}

HackServerConfiguration& EventHackSettingsListing::addServer(size_t serverId, std::string serverName) {
    servers.emplace_back(serverId, serverName);
    return servers.back();
}

size_t EventHackSettingsListing::getUserId() const {
    return userId;
}

const std::list<HackServerConfiguration>& EventHackSettingsListing::getServerList() const {
    return servers;
}

void* EventHackSettingsListing::getData() const {
    return data;
}

