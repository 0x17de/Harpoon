#include "EventIrcNoticed.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcNoticed::uuid = ::uuid.get();
UUID EventIrcNoticed::getEventUuid() const {
    return this->uuid;
}

EventIrcNoticed::EventIrcNoticed(size_t userId, size_t serverId,
                                 const std::string& username,
                                 const std::string& target,
                                 const std::string& message)
    : userId{userId}
    , serverId{serverId}
    , username{username}
    , target{target}
    , message{message}
{
}

size_t EventIrcNoticed::getUserId() const {
    return userId;
}

size_t EventIrcNoticed::getServerId() const {
    return serverId;
}

std::string EventIrcNoticed::getUsername() const {
    return username;
}

std::string EventIrcNoticed::getTarget() const {
    return target;
}

std::string EventIrcNoticed::getMessage() const {
    return message;
}

