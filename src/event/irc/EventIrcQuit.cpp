#include "EventIrcQuit.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcQuit::uuid = ::uuid.get();
UUID EventIrcQuit::getEventUuid() const {
    return this->uuid;
}

EventIrcQuit::EventIrcQuit(size_t userId, size_t serverId, const std::string& who, const std::string& reason)
    : userId{userId}
    , serverId{serverId}
    , who{who}
    , reason{reason}
{
}

size_t EventIrcQuit::getUserId() const {
    return userId;
}

size_t EventIrcQuit::getServerId() const {
    return serverId;
}

std::string EventIrcQuit::getWho() const {
    return who;
}

std::string EventIrcQuit::getReason() const {
    return reason;
}
