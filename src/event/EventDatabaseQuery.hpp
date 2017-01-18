#ifndef EVENTDATABASEQUERY_H
#define EVENTDATABASEQUERY_H

#include "IDatabaseEvent.hpp"
#include <memory>
#include <list>


namespace Query { struct QueryBase; }
class EventQueue;
class EventDatabaseQuery : public IDatabaseEvent {
    EventQueue* target;
    std::list<std::unique_ptr<Query::QueryBase>> queries;
    std::shared_ptr<IEvent> eventOrigin;

    inline void addQuery() { }
    template<class S, class... T>
    void addQuery(std::unique_ptr<Query::QueryBase>&& query, T&&... t) {
        queries.emplace_back(std::move(query));
        addQuery(std::forward<T>(t)...);
    }

public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    template<class... T>
    EventDatabaseQuery(EventQueue* target, const std::shared_ptr<IEvent>& eventOrigin, T&&... t)
        : target{target}
        , eventOrigin{eventOrigin}
    { }
    EventQueue* getTarget() const;
    std::shared_ptr<IEvent> getEventOrigin() const;
    const std::list<std::unique_ptr<Query::QueryBase>>& getQueries() const;
};

#endif
