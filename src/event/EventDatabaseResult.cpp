#include "EventDatabaseResult.hpp"
#include "utils/uuidGen.hpp"


UUID EventDatabaseResult::uuid = ::uuid.get();
UUID EventDatabaseResult::getEventUuid() const {
    return this->uuid;
}

EventDatabaseResult::EventDatabaseResult(EventQueue* target,
                                         std::shared_ptr<IEvent> eventOrigin,
                                         bool success,
                                         std::list<std::string> columns)
    : target{target}
    , success{success}
    , columns{columns}
    , eventOrigin{eventOrigin}
{
}

void EventDatabaseResult::addResult(const std::string& result) {
    results.push_back(result);
}

bool EventDatabaseResult::getSuccess() const {
    return success;
}

const std::list<std::string>& EventDatabaseResult::getResults() const {
    return results;
}
