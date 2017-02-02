#ifndef EVENTIRCSERVERADDED_H
#define EVENTIRCSERVERADDED_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcServerAdded : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string serverName;
public:
    static constexpr UUID uuid = 40;
    virtual UUID getEventUuid() const override;

    EventIrcServerAdded(size_t userId, size_t serverId, const std::string& serverName);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getServerName() const;
};

#endif
