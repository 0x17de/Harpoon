#ifndef EVENTIRCCONNECTED_H
#define EVENTIRCCONNECTED_H

#include "../IClientEvent.hpp"
#include <cstdlib>


class EventIrcConnected : public IClientEvent {
    size_t userId;
    size_t serverId;
public:
    static constexpr UUID uuid = 17;
    virtual UUID getEventUuid() const override;

    EventIrcConnected(size_t userId, size_t serverId);
    size_t getUserId() const;
    size_t getServerId() const;
};

#endif
