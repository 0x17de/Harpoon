#include "EventHackModifyNick.hpp"


EventHackModifyNick::EventHackModifyNick(size_t userId,
                                         size_t serverId,
                                         const std::string& oldNick,
                                         const std::string& newNick)
    : userId{userId}
    , serverId{serverId}
    , oldNick{oldNick}
    , newNick{newNick}
{
}

UUID EventHackModifyNick::getEventUuid() const {
    return this->uuid;
}

size_t EventHackModifyNick::getUserId() const {
    return userId;
}

size_t EventHackModifyNick::getServerId() const {
    return serverId;
}

std::string EventHackModifyNick::getOldNick() const {
    return oldNick;
}

std::string EventHackModifyNick::getNewNick() const {
    return newNick;
}
