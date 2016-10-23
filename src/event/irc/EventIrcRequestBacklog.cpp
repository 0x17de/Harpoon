#include "EventIrcRequestBacklog.hpp"
#include "utils/uuidGen.hpp"


EventIrcRequestBacklog::EventIrcRequestBacklog(size_t userId,
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

UUID EventIrcRequestBacklog::uuid = ::uuid.get();
UUID EventIrcRequestBacklog::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcRequestBacklog::getUserId() const {
    return userId;
}

size_t EventIrcRequestBacklog::getServerId() const {
    return serverId;
}

std::string EventIrcRequestBacklog::getChannelName() const {
    return channelName;
}

size_t EventIrcRequestBacklog::getFromId() const {
    return fromId;
}

int EventIrcRequestBacklog::getCount() const {
    return count;
}
