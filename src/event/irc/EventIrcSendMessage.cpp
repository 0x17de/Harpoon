#include "EventIrcSendMessage.hpp"


UUID EventIrcSendMessage::getEventUuid() const {
    return this->uuid;
}

EventIrcSendMessage::EventIrcSendMessage(size_t userId, size_t serverId, const std::string& channel, const std::string& message, MessageType type)
    :
    userId{userId},
    serverId{serverId},
    channel{channel},
    message{message},
    type{type}
{
}

size_t EventIrcSendMessage::getUserId() const {
    return userId;
}

size_t EventIrcSendMessage::getServerId() const {
    return serverId;
}

std::string EventIrcSendMessage::getChannel() const {
    return channel;
}

std::string EventIrcSendMessage::getMessage() const {
    return message;
}

MessageType EventIrcSendMessage::getType() const {
    return type;
}

