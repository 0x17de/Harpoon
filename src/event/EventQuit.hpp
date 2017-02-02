#ifndef EVENTQUIT_H
#define EVENTQUIT_H

#include "IEvent.hpp"


class EventQuit : public IEvent {
public:
    static constexpr UUID uuid = 8;
    virtual UUID getEventUuid() const override;
};

#endif
