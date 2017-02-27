#include "EventHackDeleteChannel.hpp"


EventHackDeleteChannel::EventHackDeleteChannel(size_t userId,
                                             size_t serverId,
                                             const std::string& channelName)
    : userId{userId}
    , serverId{serverId}
    , channelName{channelName}
{
}

UUID EventHackDeleteChannel::getEventUuid() const {
    return this->uuid;
}

size_t EventHackDeleteChannel::getUserId() const {
    return userId;
}

size_t EventHackDeleteChannel::getServerId() const {
    return serverId;
}

std::string EventHackDeleteChannel::getChannelName() const {
    return channelName;
}
