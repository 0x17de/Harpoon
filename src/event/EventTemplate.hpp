#ifndef EVENTTEMPLATE_H
#define EVENTTEMPLATE_H

#include "IEvent.hpp"
#include <cstdlib>


class EventTemplate : public IEvent {
    size_t userId;
    size_t serverId;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventTemplate(size_t userId, size_t serverId);
    size_t getUserId() const;
    size_t getServerId() const;
};

#endif
