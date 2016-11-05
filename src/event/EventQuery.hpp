#ifndef EVENTQUERY_H
#define EVENTQUERY_H

#include "IUserEvent.hpp"
#include "EventQueryType.hpp"
#include <cstdlib>

class EventQueue;
class EventQuery : public IUserEvent {
    size_t userId;
    void* data;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventQuery(size_t userId, void* data);

    virtual size_t getUserId() const override;
    void* getData() const;
    EventQueryType getType() const;
};

#endif
