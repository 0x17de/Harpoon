#include "EventDatabaseQuery.hpp"
#include "utils/uuidGen.hpp"


UUID EventDatabaseQuery::uuid = ::uuid.get();
UUID EventDatabaseQuery::getEventUuid() const {
    return this->uuid;
}

EventDatabaseQuery::EventDatabaseQuery(EventQueue* target, std::shared_ptr<IEvent> eventOrigin)
    : target{target}
    , eventOrigin{eventOrigin}
{
}

Database::Query& EventDatabaseQuery::addQuery(Database::Query&& query) {
    queries.push_back(query);
    return queries.back();
}

const std::list<Database::Query>& EventDatabaseQuery::getQueries() const {
    return queries;
}
