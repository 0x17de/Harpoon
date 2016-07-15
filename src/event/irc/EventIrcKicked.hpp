#ifndef EVENTIRCKICKED_H
#define EVENTIRCKICKED_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcKicked : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
    std::string target;
    std::string reason;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcKicked(size_t userId, size_t serverId, const std::string& username, const std::string& channel, const std::string& target, const std::string& reason);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getTarget() const;
    std::string getChannel() const;
    std::string getReason() const;
};

#endif
