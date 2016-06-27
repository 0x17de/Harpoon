#ifndef EVENTINIT_H
#define EVENTINIT_H

#include "IEvent.hpp"


class EventInit : public IEvent {
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;
};

#endif
