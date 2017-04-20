#ifndef EVENTTEMPLATE_H
#define EVENTTEMPLATE_H

#include "IEvent.hpp"
#include <cstdlib>

/// Template event which is used nowhere
class EventTemplate : public IEvent {
    size_t userId;
    size_t serverId;
public:
    static constexpr UUID uuid = 9;
    virtual UUID getEventUuid() const override;

    EventTemplate(size_t userId, size_t serverId);
    size_t getUserId() const;
    size_t getServerId() const;
};

#endif
