#include "EventIrcUserModeChanged.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcUserModeChanged::uuid = ::uuid.get();
UUID EventIrcUserModeChanged::getEventUuid() const {
    return this->uuid;
}

EventIrcUserModeChanged::EventIrcUserModeChanged(size_t userId, size_t serverId,
                                                 const std::string& username,
                                                 const std::string& channel,
                                                 const std::string& mode)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
    , mode{mode}
{
}

size_t EventIrcUserModeChanged::getUserId() const {
    return userId;
}

size_t EventIrcUserModeChanged::getServerId() const {
    return serverId;
}

std::string EventIrcUserModeChanged::getUsername() const {
    return username;
}

std::string EventIrcUserModeChanged::getChannel() const {
    return channel;
}

std::string EventIrcUserModeChanged::getUserMode() const {
    return mode;
}

