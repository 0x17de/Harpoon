#include "EventIrcUserlistReceived.hpp"


EventIrcUserlistReceived::User::User(const std::string& nick,
                                     const std::string& mode)
    : nick{nick}
    , mode{mode}
{ }

UUID EventIrcUserlistReceived::getEventUuid() const {
    return this->uuid;
}

EventIrcUserlistReceived::EventIrcUserlistReceived(size_t userId,
                                                   size_t serverId,
                                                   const std::string& channel)
    : userId{userId}
    , serverId{serverId}
    , channel{channel}
{
}

void EventIrcUserlistReceived::addUser(const std::string& nick,
                                       const std::string& mode) {
    users.emplace_back(nick, mode);
}

size_t EventIrcUserlistReceived::getUserId() const {
    return userId;
}

size_t EventIrcUserlistReceived::getServerId() const {
    return serverId;
}

std::string EventIrcUserlistReceived::getChannel() const {
    return channel;
}

const std::list<EventIrcUserlistReceived::User>& EventIrcUserlistReceived::getUsers() const {
    return users;
}
