#include "EventLogout.hpp"
#include "utils/uuidGen.hpp"


UUID EventLogout::uuid = ::uuid.get();
UUID EventLogout::getEventUuid() const {
    return this->uuid;
}

EventLogout::EventLogout(void* data)
    : data{data}
{
}

void* EventLogout::getData() const {
    return data;
}

