#ifndef EVENTLOGOUT_H
#define EVENTLOGOUT_H

#include "IEvent.hpp"
#include <cstdlib>


class EventLogout : public IEvent {
    void* data;
public:
    static constexpr UUID uuid = 6;
    virtual UUID getEventUuid() const override;

    explicit EventLogout(void* data);
    void* getData() const;
};

#endif
