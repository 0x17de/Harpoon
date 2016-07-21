#include "EventIrcDeleteNick.hpp"
#include "utils/uuidGen.hpp"


EventIrcDeleteNick::EventIrcDeleteNick(size_t userId, size_t serverId, const std::string& nick)
    : userId{userId}
    , serverId{serverId}
    , nick{nick}
{
}

UUID EventIrcDeleteNick::uuid = ::uuid.get();
UUID EventIrcDeleteNick::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcDeleteNick::getUserId() const {
    return userId;
}

size_t EventIrcDeleteNick::getServerId() const {
    return serverId;
}

std::string EventIrcDeleteNick::getNick() const {
    return nick;
}
