#include "EventHackBacklogResponse.hpp"
#include "service/hack/HackDatabaseMessageType.hpp"


UUID EventHackBacklogResponse::getEventUuid() const {
    return this->uuid;
}

EventHackBacklogResponse::EventHackBacklogResponse(size_t userId,
                                                   size_t serverId,
                                                   const std::string& channel,
                                                   std::list<HackMessageData>&& events)
    : userId{userId}
    , serverId{serverId}
    , channel{channel}
    , events{std::move(events)}
{
}

size_t EventHackBacklogResponse::getUserId() const {
    return userId;
}

size_t EventHackBacklogResponse::getServerId() const {
    return serverId;
}

std::string EventHackBacklogResponse::getChannel() const {
    return channel;
}

const std::list<HackMessageData>& EventHackBacklogResponse::getData() const {
    return events;
}
