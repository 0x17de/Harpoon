#include "EventIrcChangeNick.hpp"


EventIrcChangeNick::EventIrcChangeNick(size_t userId,
                                       size_t serverId,
                                       const std::string& nick)
    : userId{userId}
    , serverId{serverId}
    , nick{nick}
{
}

UUID EventIrcChangeNick::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcChangeNick::getUserId() const {
    return userId;
}

size_t EventIrcChangeNick::getServerId() const {
    return serverId;
}

std::string EventIrcChangeNick::getNick() const {
    return nick;
}
