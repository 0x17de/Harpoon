#include "EventIrcNumeric.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcNumeric::uuid = ::uuid.get();
UUID EventIrcNumeric::getEventUuid() const {
    return this->uuid;
}

EventIrcNumeric::EventIrcNumeric(size_t userId, size_t serverId,
                                 unsigned int eventCode,
                                 const std::string& from,
                                 const std::vector<std::string>& parameters)
    : userId{userId}
    , serverId{serverId}
    , eventCode{eventCode}
    , from{from}
    , parameters{parameters}
{
}

size_t EventIrcNumeric::getUserId() const {
    return userId;
}

size_t EventIrcNumeric::getServerId() const {
    return serverId;
}

unsigned int EventIrcNumeric::getEventCode() const {
    return eventCode;
}

std::string EventIrcNumeric::getFrom() const {
    return from;
}

const std::vector<std::string>& EventIrcNumeric::getParameters() const {
    return parameters;
}

