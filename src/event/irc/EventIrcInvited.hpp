#ifndef EVENTIRCINVITED_H
#define EVENTIRCINVITED_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcInvited : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string target;
    std::string channel;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcInvited(size_t userId, size_t serverId,
                    const std::string& username,
                    const std::string& target,
                    const std::string& channel);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getTarget() const;
    std::string getChannel() const;
};

#endif
