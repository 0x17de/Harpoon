#include "EventIrcBacklogResponse.hpp"
#include "service/irc/IrcDatabaseMessageType.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcBacklogResponse::uuid = ::uuid.get();
UUID EventIrcBacklogResponse::getEventUuid() const {
    return this->uuid;
}

EventIrcBacklogResponse::EventIrcBacklogResponse(size_t userId,
                                                 size_t serverId,
                                                 const std::string& channel,
                                                 std::list<MessageData>&& events)
    : userId{userId}
    , serverId{serverId}
    , channel{channel}
    , events{std::move(events)}
{
}

size_t EventIrcBacklogResponse::getUserId() const {
    return userId;
}

size_t EventIrcBacklogResponse::getServerId() const {
    return serverId;
}

std::string EventIrcBacklogResponse::getChannel() const {
    return channel;
}

const std::list<MessageData>& EventIrcBacklogResponse::getData() const {
    return events;
}
