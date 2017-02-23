#include "EventHackPartChannel.hpp"


EventHackPartChannel::EventHackPartChannel(size_t userId,
                                           size_t serverId,
                                           const std::string& channel)
    : userId{userId}
    , serverId{serverId}
    , channel{channel}
{
}

UUID EventHackPartChannel::getEventUuid() const {
    return this->uuid;
}

size_t EventHackPartChannel::getUserId() const {
    return userId;
}

size_t EventHackPartChannel::getServerId() const {
    return serverId;
}

std::string EventHackPartChannel::getChannel() const {
    return channel;
}
