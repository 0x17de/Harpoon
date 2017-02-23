#include "EventHackJoinChannel.hpp"


EventHackJoinChannel::EventHackJoinChannel(size_t userId, size_t serverId, const std::string& channel)
    : userId{userId}
    , serverId{serverId}
    , channel{channel}
{
}

UUID EventHackJoinChannel::getEventUuid() const {
    return this->uuid;
}

size_t EventHackJoinChannel::getUserId() const {
    return userId;
}

size_t EventHackJoinChannel::getServerId() const {
    return serverId;
}

std::string EventHackJoinChannel::getChannel() const {
    return channel;
}
