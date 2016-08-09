#ifndef EVENTDATABASERESULT_H
#define EVENTDATABASERESULT_H

#include "IDatabaseEvent.hpp"
#include "db/DatabaseQuery.hpp"
#include <memory>
#include <list>


class EventQueue;
class EventDatabaseResult : public IDatabaseEvent {
    EventQueue* target;
    bool success;
    std::list<std::string> columns;
    std::shared_ptr<IEvent> eventOrigin;
    std::list<std::string> results;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventDatabaseResult(EventQueue* target,
                        std::shared_ptr<IEvent> eventOrigin,
                        bool success,
                        std::list<std::string> columns);
    void addResult(const std::string& result);
    bool getSuccess() const;
    const std::list<std::string>& getResults() const;
};

#endif
