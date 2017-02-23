#include "EventHackSendMessage.hpp"


UUID EventHackSendMessage::getEventUuid() const {
    return this->uuid;
}

EventHackSendMessage::EventHackSendMessage(size_t userId,
                                           size_t serverId,
                                           const std::string& channel,
                                           const std::string& message)
    :
    userId{userId},
    serverId{serverId},
    channel{channel},
    message{message}
{
}

size_t EventHackSendMessage::getUserId() const {
    return userId;
}

size_t EventHackSendMessage::getServerId() const {
    return serverId;
}

std::string EventHackSendMessage::getChannel() const {
    return channel;
}

std::string EventHackSendMessage::getMessage() const {
    return message;
}
