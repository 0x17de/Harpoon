#include "EventHackRequestBacklog.hpp"


EventHackRequestBacklog::EventHackRequestBacklog(size_t userId,
                                                 size_t serverId,
                                                 const std::string& channelName,
                                                 size_t fromId,
                                                 int count)
    : userId{userId}
    , serverId{serverId}
    , channelName{channelName}
    , fromId{fromId}
    , count{count}
{
}

UUID EventHackRequestBacklog::getEventUuid() const {
    return this->uuid;
}

size_t EventHackRequestBacklog::getUserId() const {
    return userId;
}

size_t EventHackRequestBacklog::getServerId() const {
    return serverId;
}

std::string EventHackRequestBacklog::getChannelName() const {
    return channelName;
}

size_t EventHackRequestBacklog::getFromId() const {
    return fromId;
}

int EventHackRequestBacklog::getCount() const {
    return count;
}
