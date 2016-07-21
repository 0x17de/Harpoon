#include "EventIrcModifyNick.hpp"
#include "utils/uuidGen.hpp"


EventIrcModifyNick::EventIrcModifyNick(size_t userId,
                                       size_t serverId,
                                       const std::string& oldNick,
                                       const std::string& newNick)
    : userId{userId}
    , serverId{serverId}
    , oldNick{oldNick}
    , newNick{newNick}
{
}

UUID EventIrcModifyNick::uuid = ::uuid.get();
UUID EventIrcModifyNick::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcModifyNick::getUserId() const {
    return userId;
}

size_t EventIrcModifyNick::getServerId() const {
    return serverId;
}

std::string EventIrcModifyNick::getOldNick() const {
    return oldNick;
}

std::string EventIrcModifyNick::getNewNick() const {
    return newNick;
}
