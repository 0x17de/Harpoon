#include "EventIrcNickChanged.hpp"


UUID EventIrcNickChanged::getEventUuid() const {
    return this->uuid;
}

EventIrcNickChanged::EventIrcNickChanged(size_t userId,
										 size_t serverId,
										 const std::string& username,
										 const std::string& newNick)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , newNick{newNick}
{
}

size_t EventIrcNickChanged::getUserId() const {
    return userId;
}

size_t EventIrcNickChanged::getServerId() const {
    return serverId;
}

std::string EventIrcNickChanged::getUsername() const {
    return username;
}

std::string EventIrcNickChanged::getNewNick() const {
    return newNick;
}

