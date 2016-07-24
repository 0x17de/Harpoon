#include "EventIrcNickModified.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcNickModified::uuid = ::uuid.get();
UUID EventIrcNickModified::getEventUuid() const {
    return this->uuid;
}

EventIrcNickModified::EventIrcNickModified(size_t userId,
										 size_t serverId,
										 const std::string& oldNick,
										 const std::string& newNick)
    : userId{userId}
    , serverId{serverId}
    , oldNick{oldNick}
    , newNick{newNick}
{
}

size_t EventIrcNickModified::getUserId() const {
    return userId;
}

size_t EventIrcNickModified::getServerId() const {
    return serverId;
}

std::string EventIrcNickModified::getOldNick() const {
    return oldNick;
}

std::string EventIrcNickModified::getNewNick() const {
    return newNick;
}

