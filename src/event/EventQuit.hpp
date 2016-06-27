#ifndef EVENTQUIT_H
#define EVENTQUIT_H

#include "IEvent.hpp"


class EventQuit : public IEvent {
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;
};

#endif
