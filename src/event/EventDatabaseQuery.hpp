#ifndef EVENTDATABASEQUERY_H
#define EVENTDATABASEQUERY_H

#include "IDatabaseEvent.hpp"
#include <memory>
#include <list>
#include "db/query/Database_QueryBase.hpp"


namespace Query { struct QueryBase; }
class EventQueue;
/// Event for abstract database queries to a database module
class EventDatabaseQuery : public IDatabaseEvent {
    EventQueue* target;
    std::list<std::unique_ptr<Query::QueryBase>> queries;
    std::shared_ptr<IEvent> eventOrigin;

    /// termination condition
    template<int n, class... T>
    inline typename std::enable_if<n == sizeof...(T)>::type addQueries(std::tuple<T...>& query) {
    }
    /// recursively adds queries
    template<int n, class... T>
    inline typename std::enable_if<n != sizeof...(T)>::type addQueries(std::tuple<T...>& query) {
        queries.push_back(std::move(std::get<n>(query)));
        addQueries<n+1>(query);
    }

public:
    static constexpr UUID uuid = 1;
    virtual UUID getEventUuid() const override;

    /// Constructor adding all supplied queries into the event
    template<class... T>
    EventDatabaseQuery(EventQueue* target, const std::shared_ptr<IEvent>& eventOrigin, T&&... t)
        : target{target}
        , eventOrigin{eventOrigin}
    {
        std::tuple<T&...> tuple(t...);
        addQueries<0>(tuple);
    }

    /// Returns the requesting event queue. The only target that will be notified by the results
    EventQueue* getTarget() const;
    /// Returns the original event that was initiator for the database request. E.g. A chat message event.
    std::shared_ptr<IEvent> getEventOrigin() const;
    /// Returns the const reference to all queries
    const std::list<std::unique_ptr<Query::QueryBase>>& getQueries() const;
};

#endif
