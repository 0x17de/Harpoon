#ifndef EVENTHACKPARTED_H
#define EVENTHACKPARTED_H

#include "../IClientEvent.hpp"
#include "HackLoggable.hpp"
#include <string>


class EventHackParted : public IClientEvent, public HackLoggable {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
public:
    static constexpr UUID uuid = 61;
    virtual UUID getEventUuid() const override;

    EventHackParted(size_t userId, size_t serverId, const std::string& username, const std::string& channel);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
};

#endif
