#include "EventHackJoined.hpp"


UUID EventHackJoined::getEventUuid() const {
    return this->uuid;
}

EventHackJoined::EventHackJoined(size_t userId,
                                 size_t serverId,
                                 const std::string& username,
                                 const std::string& channel)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , channel{channel}
{
}

size_t EventHackJoined::getUserId() const {
    return userId;
}

size_t EventHackJoined::getServerId() const {
    return serverId;
}

std::string EventHackJoined::getUsername() const {
    return username;
}

std::string EventHackJoined::getChannel() const {
    return channel;
}

