#ifndef EVENTDATABASEQUERY_H
#define EVENTDATABASEQUERY_H

#include "IDatabaseEvent.hpp"
#include "db/DatabaseQuery.hpp"
#include <memory>
#include <list>


class EventQueue;
class EventDatabaseQuery : public IDatabaseEvent {
    EventQueue* target;
    std::list<Database::Query> queries;
    std::shared_ptr<IEvent> eventOrigin;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventDatabaseQuery(EventQueue* target, std::shared_ptr<IEvent> eventOrigin);
    Database::Query& addQuery(Database::Query&& query);
    const std::list<Database::Query>& getQueries() const;
};

#endif
