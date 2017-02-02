#include "EventQuery.hpp"


UUID EventQuery::getEventUuid() const {
    return this->uuid;
}

EventQuery::EventQuery(size_t userId, void* data, EventQueryType type)
    : userId{userId}
    , data{data}
    , type{type}
{
}

size_t EventQuery::getUserId() const {
    return userId;
}

void* EventQuery::getData() const {
    return data;
}

EventQueryType EventQuery::getType() const {
    return type;
}
