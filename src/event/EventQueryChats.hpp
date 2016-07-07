#ifndef EVENTQUERYCHATS_H
#define EVENTQUERYCHATS_H

#include "IUserEvent.hpp"
#include <cstdlib>


class EventQueue;
class EventQueryChats : public IUserEvent {
    size_t userId;
    EventQueue* target;
    void* data;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventQueryChats(size_t userId, void* data);

    virtual size_t getUserId() const override;
    void* getData() const;
};

#endif
