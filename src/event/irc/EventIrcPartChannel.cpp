#include "EventIrcPartChannel.hpp"
#include "utils/uuidGen.hpp"


EventIrcPartChannel::EventIrcPartChannel(size_t userId,
                                         size_t serverId)
    : userId{userId}
    , serverId{serverId}
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

void EventIrcPartChannel::addChannel(const std::string& channel) {
    channels.push_back(channel);
}

const std::list<std::string> EventIrcPartChannel::getChannels() const {
    return channels;
}
