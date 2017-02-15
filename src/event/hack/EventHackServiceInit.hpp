#ifndef EVENTHACKSERVICEINIT_H
#define EVENTHACKSERVICEINIT_H

#include "../IEvent.hpp"


class EventHackServiceInit : public IEvent {
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;
};

#endif
