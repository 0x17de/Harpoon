#ifndef EVENTHACKCONNECTED_H
#define EVENTHACKCONNECTED_H

#include "../IClientEvent.hpp"
#include <cstdlib>


class EventHackConnected : public IClientEvent {
    size_t userId;
    size_t serverId;
public:
    static constexpr UUID uuid = 50;
    virtual UUID getEventUuid() const override;

    EventHackConnected(size_t userId, size_t serverId);
    size_t getUserId() const;
    size_t getServerId() const;
};

#endif
