#include "EventLogout.hpp"


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

