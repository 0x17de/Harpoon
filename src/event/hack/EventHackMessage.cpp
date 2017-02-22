#include "EventHackMessage.hpp"


UUID EventHackMessage::getEventUuid() const {
    return this->uuid;
}

EventHackMessage::EventHackMessage(size_t userId,
                                   size_t serverId,
                                   const std::string& from,
                                   const std::string& channel,
                                   const std::string& message,
                                   HackMessageType type)
    : userId{userId}
    , serverId{serverId}
    , from{from}
    , channel{channel}
    , message{message}
    , type{type}
{
}

size_t EventHackMessage::getUserId() const {
    return userId;
}

size_t EventHackMessage::getServerId() const {
    return serverId;
}

std::string EventHackMessage::getFrom() const {
    return from;
}

std::string EventHackMessage::getChannel() const {
    return channel;
}

std::string EventHackMessage::getMessage() const {
    return message;
}

HackMessageType EventHackMessage::getType() const {
    return type;
}
