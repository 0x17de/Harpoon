#include "EventIrcUserStatusRequest.hpp"
#include "service/irc/IrcChannelLoginData.hpp"


EventIrcUserStatusRequest::EventIrcUserStatusRequest(Status status,
                                                     size_t userId,
                                                     size_t serverId,
                                                     const std::string& target,
                                                     const std::string& password)
    : status{status}
    , userId{userId}
    , serverId{serverId}
    , target{target}
    , password{password}
{
}

UUID EventIrcUserStatusRequest::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcUserStatusRequest::getUserId() const {
    return userId;
}

size_t EventIrcUserStatusRequest::getServerId() const {
    return serverId;
}

EventIrcUserStatusRequest::Status EventIrcUserStatusRequest::getStatus() const {
    return status;
}

std::string EventIrcUserStatusRequest::getTarget() const {
    return target;
}
std::string EventIrcUserStatusRequest::getPassword() const {
    return password;
}
