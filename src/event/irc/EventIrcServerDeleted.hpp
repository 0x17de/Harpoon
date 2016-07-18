#ifndef EVENTIRCSERVERDELETED_H
#define EVENTIRCSERVERDELETED_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcServerDeleted : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string serverName;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcServerDeleted(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
};

#endif
