#ifndef EVENTIRCSERVICEINIT_H
#define EVENTIRCSERVICEINIT_H

#include "../IEvent.hpp"


class EventIrcServiceInit : public IEvent {
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;
};

#endif
