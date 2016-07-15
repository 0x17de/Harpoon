#ifndef EVENTIRCPARTED_H
#define EVENTIRCPARTED_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcParted : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcParted(size_t userId, size_t serverId, const std::string& username, const std::string& channel);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
};

#endif
