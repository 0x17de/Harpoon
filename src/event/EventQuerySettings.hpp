#ifndef EVENTQUERYSETTINGS_H
#define EVENTQUERYSETTINGS_H

#include "IUserEvent.hpp"
#include <cstdlib>


class EventQueue;
class EventQuerySettings : public IUserEvent {
    size_t userId;
    EventQueue* target;
    void* data;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventQuerySettings(size_t userId, void* data);

    virtual size_t getUserId() const override;
    void* getData() const;
};

#endif
