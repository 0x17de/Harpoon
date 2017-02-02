#include "EventIrcSendAction.hpp"


UUID EventIrcSendAction::getEventUuid() const {
    return this->uuid;
}

EventIrcSendAction::EventIrcSendAction(size_t userId, size_t serverId, const std::string& channel, const std::string& message)
    :
    userId{userId},
    serverId{serverId},
    channel{channel},
    message{message}
{
}

size_t EventIrcSendAction::getUserId() const {
    return userId;
}

size_t EventIrcSendAction::getServerId() const {
    return serverId;
}

std::string EventIrcSendAction::getChannel() const {
    return channel;
}

std::string EventIrcSendAction::getMessage() const {
    return message;
}

