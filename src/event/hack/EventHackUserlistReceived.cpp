#include "EventHackUserlistReceived.hpp"


EventHackUserlistReceived::User::User(const std::string& nick)
    : nick{nick}
{ }

UUID EventHackUserlistReceived::getEventUuid() const {
    return this->uuid;
}

EventHackUserlistReceived::EventHackUserlistReceived(size_t userId,
                                                   size_t serverId,
                                                   const std::string& channel)
    : userId{userId}
    , serverId{serverId}
    , channel{channel}
{
}

void EventHackUserlistReceived::addUser(const std::string& nick) {
    users.emplace_back(nick);
}

size_t EventHackUserlistReceived::getUserId() const {
    return userId;
}

size_t EventHackUserlistReceived::getServerId() const {
    return serverId;
}

std::string EventHackUserlistReceived::getChannel() const {
    return channel;
}

const std::list<EventHackUserlistReceived::User>& EventHackUserlistReceived::getUsers() const {
    return users;
}
