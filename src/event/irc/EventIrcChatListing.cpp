#include "EventIrcChatListing.hpp"
#include "IrcServerListing.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcChatListing::uuid = ::uuid.get();
UUID EventIrcChatListing::getEventUuid() const {
    return this->uuid;
}

EventIrcChatListing::EventIrcChatListing(size_t firstId,
                                         size_t userId,
                                         void* data)
    : firstId{firstId}
    , userId{userId}
    , data{data}
{
}

IrcServerListing& EventIrcChatListing::addServer(std::string activeNick,
                                                 size_t serverId,
                                                 std::string serverName) {
    servers.emplace_back(activeNick, serverId, serverName);
    return servers.back();
}

size_t EventIrcChatListing::getFirstId() const {
    return firstId;
}

size_t EventIrcChatListing::getUserId() const {
    return userId;
}

const std::list<IrcServerListing>& EventIrcChatListing::getServerList() const {
    return servers;
}

void* EventIrcChatListing::getData() const {
    return data;
}

