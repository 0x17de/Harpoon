#ifndef EVENTIRCSERVICEINIT_H
#define EVENTIRCSERVICEINIT_H

#include "../IEvent.hpp"


class EventIrcServiceInit : public IEvent {
public:
    static constexpr UUID uuid = 42;
    virtual UUID getEventUuid() const override;
};

#endif
