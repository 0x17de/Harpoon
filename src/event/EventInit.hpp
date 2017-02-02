#ifndef EVENTINIT_H
#define EVENTINIT_H

#include "IEvent.hpp"


class EventInit : public IEvent {
public:
    static constexpr UUID uuid = 3;
    virtual UUID getEventUuid() const override;
};

#endif
