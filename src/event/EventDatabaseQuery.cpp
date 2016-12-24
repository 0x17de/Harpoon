#include "EventDatabaseQuery.hpp"
#include "utils/uuidGen.hpp"


UUID EventDatabaseQuery::uuid = ::uuid.get();
UUID EventDatabaseQuery::getEventUuid() const {
    return this->uuid;
}

EventDatabaseQuery::EventDatabaseQuery(EventQueue* target, const std::shared_ptr<IEvent>& eventOrigin)
    : target{target}
    , eventOrigin{eventOrigin}
{
}

Database::Query& EventDatabaseQuery::add(Database::Query&& query) {
    queries.push_back(query);
    return queries.back();
}

EventQueue* EventDatabaseQuery::getTarget() const {
    return target;
}

std::shared_ptr<IEvent> EventDatabaseQuery::getEventOrigin() const {
    return eventOrigin;
}

const std::list<Database::Query>& EventDatabaseQuery::getQueries() const {
    return queries;
}
