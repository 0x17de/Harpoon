#ifndef EVENTHACKSERVERADDED_H
#define EVENTHACKSERVERADDED_H

#include "../IClientEvent.hpp"
#include <string>


class EventHackServerAdded : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string serverName;
public:
    static constexpr UUID uuid = 65;
    virtual UUID getEventUuid() const override;

    EventHackServerAdded(size_t userId, size_t serverId, const std::string& serverName);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getServerName() const;
};

#endif
