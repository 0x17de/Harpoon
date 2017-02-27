#ifndef EVENTHACKSERVERDELETED_H
#define EVENTHACKSERVERDELETED_H

#include "../IClientEvent.hpp"
#include <string>


class EventHackServerDeleted : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string serverName;
public:
    static constexpr UUID uuid = 66;
    virtual UUID getEventUuid() const override;

    EventHackServerDeleted(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
};

#endif
