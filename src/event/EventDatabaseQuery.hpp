#ifndef EVENTDATABASEQUERY_H
#define EVENTDATABASEQUERY_H

#include "IDatabaseEvent.hpp"
#include <memory>
#include <list>
#include "db/query/Database_QueryBase.hpp"


namespace Query { struct QueryBase; }
class EventQueue;
class EventDatabaseQuery : public IDatabaseEvent {
    EventQueue* target;
    std::list<std::unique_ptr<Query::QueryBase>> queries;
    std::shared_ptr<IEvent> eventOrigin;

    template<int n, class... T>
    typename std::enable_if<n == sizeof...(T)>::type addQueries(std::tuple<T...>& query) {
    }
    template<int n, class... T>
    typename std::enable_if<n != sizeof...(T)>::type addQueries(std::tuple<T...>& query) {
        queries.push_back(std::move(std::get<n>(query)));
        addQueries<n+1>(query);
    }

public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    template<class... T>
    EventDatabaseQuery(EventQueue* target, const std::shared_ptr<IEvent>& eventOrigin, T&&... t)
        : target{target}
        , eventOrigin{eventOrigin}
    {
        std::tuple<T&...> tuple(t...);
        addQueries<0>(tuple);
    }

    EventQueue* getTarget() const;
    std::shared_ptr<IEvent> getEventOrigin() const;
    const std::list<std::unique_ptr<Query::QueryBase>>& getQueries() const;
};

#endif
