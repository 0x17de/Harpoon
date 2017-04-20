#ifndef EVENTINIT_H
#define EVENTINIT_H

#include "IEvent.hpp"

/// Event which is sent to all modules on initialization at once
class EventInit : public IEvent {
public:
    static constexpr UUID uuid = 3;
    virtual UUID getEventUuid() const override;
};

#endif
