#include "IEvent.hpp"


IEvent::IEvent()
    : timestamp(std::chrono::system_clock::now())
{
}

decltype(IEvent::timestamp) IEvent::getTimestamp() const {
    return timestamp;
}
