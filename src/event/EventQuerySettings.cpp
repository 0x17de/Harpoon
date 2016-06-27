#include "EventQuerySettings.hpp"
#include "utils/uuidGen.hpp"


UUID EventQuerySettings::uuid = ::uuid.get();
UUID EventQuerySettings::getEventUuid() const {
    return this->uuid;
}

EventQuerySettings::EventQuerySettings(size_t userId, void* data)
    : userId{userId}
    , data{data}
{
}

size_t EventQuerySettings::getUserId() const {
    return userId;
}

void* EventQuerySettings::getData() const {
    return data;
}

