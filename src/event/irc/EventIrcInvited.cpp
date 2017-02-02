#include "EventIrcInvited.hpp"


UUID EventIrcInvited::getEventUuid() const {
    return this->uuid;
}

EventIrcInvited::EventIrcInvited(size_t userId, size_t serverId,
                                 const std::string& username,
                                 const std::string& target,
                                 const std::string& channel)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , target{target}
    , channel{channel}
{
}

size_t EventIrcInvited::getUserId() const {
    return userId;
}

size_t EventIrcInvited::getServerId() const {
    return serverId;
}

std::string EventIrcInvited::getUsername() const {
    return username;
}

std::string EventIrcInvited::getTarget() const {
    return target;
}

std::string EventIrcInvited::getChannel() const {
    return channel;
}

