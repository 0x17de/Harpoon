#include "EventDatabaseQuery.hpp"


UUID EventDatabaseQuery::getEventUuid() const {
    return this->uuid;
}

EventQueue* EventDatabaseQuery::getTarget() const {
    return target;
}

std::shared_ptr<IEvent> EventDatabaseQuery::getEventOrigin() const {
    return eventOrigin;
}

const std::list<std::unique_ptr<Query::QueryBase>>& EventDatabaseQuery::getQueries() const {
    return queries;
}
