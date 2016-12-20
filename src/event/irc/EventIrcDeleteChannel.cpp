#include "EventIrcDeleteChannel.hpp"
#include "utils/uuidGen.hpp"


EventIrcDeleteChannel::EventIrcDeleteChannel(size_t userId,
                                             size_t serverId,
                                             std::string channelName)
    : userId{userId}
    , serverId{serverId}
    , channelName{channelName}
{
}

UUID EventIrcDeleteChannel::uuid = ::uuid.get();
UUID EventIrcDeleteChannel::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcDeleteChannel::getUserId() const {
    return userId;
}

size_t EventIrcDeleteChannel::getServerId() const {
    return serverId;
}

std::string EventIrcDeleteChannel::getChannelName() const {
    return channelName;
}
