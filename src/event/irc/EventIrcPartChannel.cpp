#include "EventIrcPartChannel.hpp"
#include "utils/uuidGen.hpp"


EventIrcPartChannel::EventIrcPartChannel(size_t userId,
                                         size_t serverId,
                                         std::list<std::string> partChannels)
    : userId{userId}
    , serverId{serverId}
    , channels{channels}
{
}

UUID EventIrcPartChannel::uuid = ::uuid.get();
UUID EventIrcPartChannel::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcPartChannel::getUserId() const {
    return userId;
}

size_t EventIrcPartChannel::getServerId() const {
    return serverId;
}

const std::list<std::string> EventIrcPartChannel::getChannels() const {
    return channels;
}

