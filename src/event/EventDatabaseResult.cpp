#include "EventDatabaseResult.hpp"


UUID EventDatabaseResult::getEventUuid() const {
    return this->uuid;
}

EventDatabaseResult::EventDatabaseResult(std::shared_ptr<IEvent> eventOrigin)
    : success{false}
    , eventOrigin{eventOrigin}
{
}

void EventDatabaseResult::setSuccess(bool lsuccess) {
    success = lsuccess;
}

void EventDatabaseResult::addResult(const std::string& result) {
    results.push_back(result);
}

bool EventDatabaseResult::getSuccess() const {
    return success;
}

std::shared_ptr<IEvent> EventDatabaseResult::getEventOrigin() const {
    return eventOrigin;
}

const std::list<std::string>& EventDatabaseResult::getResults() const {
    return results;
}
