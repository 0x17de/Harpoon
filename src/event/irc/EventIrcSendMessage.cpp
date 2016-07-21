#include "EventIrcSendMessage.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcSendMessage::uuid = ::uuid.get();
UUID EventIrcSendMessage::getEventUuid() const {
    return this->uuid;
}

EventIrcSendMessage::EventIrcSendMessage(size_t userId, size_t serverId, const std::string& channel, const std::string& message)
    :
    userId{userId},
    serverId{serverId},
    channel{channel},
    message{message}
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

