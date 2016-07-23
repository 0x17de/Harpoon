#ifndef EVENTIRCUSERLISTRECEIVED_H
#define EVENTIRCUSERLISTRECEIVED_H

#include "../IClientEvent.hpp"
#include <string>
#include <list>


class EventIrcUserlistReceived : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string channel;
    std::list<std::string> users;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcUserlistReceived(size_t userId,
                             size_t serverId,
                             const std::string& channel);
    void addUser(const std::string& user);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    const std::list<std::string>& getUsers() const;
};

#endif
