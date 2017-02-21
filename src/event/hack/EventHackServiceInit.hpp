#ifndef EVENTHACKSERVICEINIT_H
#define EVENTHACKSERVICEINIT_H

#include "../IEvent.hpp"


class EventHackServiceInit : public IEvent {
public:
    static constexpr UUID uuid = 48;
    virtual UUID getEventUuid() const override;
};

#endif
