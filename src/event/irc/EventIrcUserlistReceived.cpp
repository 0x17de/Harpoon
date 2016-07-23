#include "EventIrcUserlistReceived.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcUserlistReceived::uuid = ::uuid.get();
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

void EventIrcUserlistReceived::addUser(const std::string& user) {
    users.push_back(user);
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

const std::list<std::string>& EventIrcUserlistReceived::getUsers() const {
    return users;
}
