#include "EventDatabaseResult.hpp"
#include "utils/uuidGen.hpp"


UUID EventDatabaseResult::uuid = ::uuid.get();
UUID EventDatabaseResult::getEventUuid() const {
    return this->uuid;
}

EventDatabaseResult::EventDatabaseResult(EventQueue* target, std::shared_ptr<IEvent> eventOrigin)
    : target{target}
    , eventOrigin{eventOrigin}
{
}

void EventDatabaseResult::addResult(const std::string& result) {
    results.push_back(result);
}

const std::list<std::string>& EventDatabaseResult::getResults() const {
    return results;
}
