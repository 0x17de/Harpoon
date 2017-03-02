#include "EventHackNickModified.hpp"


UUID EventHackNickModified::getEventUuid() const {
    return this->uuid;
}

EventHackNickModified::EventHackNickModified(size_t userId,
                                             size_t serverId,
                                             const std::string& oldNick,
                                             const std::string& newNick)
    : userId{userId}
    , serverId{serverId}
    , oldNick{oldNick}
    , newNick{newNick}
{
}

size_t EventHackNickModified::getUserId() const {
    return userId;
}

size_t EventHackNickModified::getServerId() const {
    return serverId;
}

std::string EventHackNickModified::getOldNick() const {
    return oldNick;
}

std::string EventHackNickModified::getNewNick() const {
    return newNick;
}

