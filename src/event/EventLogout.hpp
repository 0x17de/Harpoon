#ifndef EVENTLOGOUT_H
#define EVENTLOGOUT_H

#include "IEvent.hpp"
#include <cstdlib>


class EventLogout : public IEvent {
    void* data;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventLogout(void* data);
    void* getData() const;
};

#endif
