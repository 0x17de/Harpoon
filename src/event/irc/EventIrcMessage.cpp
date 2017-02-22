#include "EventIrcMessage.hpp"


UUID EventIrcMessage::getEventUuid() const {
    return this->uuid;
}

EventIrcMessage::EventIrcMessage(size_t userId,
                                 size_t serverId,
                                 const std::string& from,
                                 const std::string& channel,
                                 const std::string& message,
                                 IrcMessageType type)
    : userId{userId}
    , serverId{serverId}
    , from{from}
    , channel{channel}
    , message{message}
    , type{type}
{
}

size_t EventIrcMessage::getUserId() const {
    return userId;
}

size_t EventIrcMessage::getServerId() const {
    return serverId;
}

std::string EventIrcMessage::getFrom() const {
    return from;
}

std::string EventIrcMessage::getChannel() const {
    return channel;
}

std::string EventIrcMessage::getMessage() const {
    return message;
}

IrcMessageType EventIrcMessage::getType() const {
    return type;
}
