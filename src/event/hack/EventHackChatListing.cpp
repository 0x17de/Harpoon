#include "EventHackChatListing.hpp"
#include "HackServerListing.hpp"


UUID EventHackChatListing::getEventUuid() const {
    return this->uuid;
}

EventHackChatListing::EventHackChatListing(size_t firstId,
                                           size_t userId,
                                           void* data)
    : firstId{firstId}
    , userId{userId}
    , data{data}
{
}

HackServerListing& EventHackChatListing::addServer(const std::string& activeNick,
                                                   size_t serverId,
                                                   const std::string& serverName) {
    servers.emplace_back(activeNick, serverId, serverName);
    return servers.back();
}

size_t EventHackChatListing::getFirstId() const {
    return firstId;
}

size_t EventHackChatListing::getUserId() const {
    return userId;
}

const std::list<HackServerListing>& EventHackChatListing::getServerList() const {
    return servers;
}

void* EventHackChatListing::getData() const {
    return data;
}

