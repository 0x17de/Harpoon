#ifndef EVENTDATABASERESULT_H
#define EVENTDATABASERESULT_H

#include "IDatabaseEvent.hpp"
#include <memory>
#include <list>


class EventQueue;
class EventDatabaseResult : public IDatabaseEvent {
    bool success;
    std::shared_ptr<IEvent> eventOrigin;
    std::list<std::string> results;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    explicit EventDatabaseResult(std::shared_ptr<IEvent> eventOrigin);
    void setSuccess(bool success);
    void addResult(const std::string& result);
    bool getSuccess() const;
    std::shared_ptr<IEvent> getEventOrigin() const;
    const std::list<std::string>& getResults() const;
};

#endif
