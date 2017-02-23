#ifndef EVENTHACKUSERLISTRECEIVED_H
#define EVENTHACKUSERLISTRECEIVED_H

#include "../IClientEvent.hpp"
#include <string>
#include <list>


class EventHackUserlistReceived : public IClientEvent {
    struct User {
        User(const std::string& nick);
        std::string nick;
    };

    size_t userId;
    size_t serverId;
    std::string channel;
    std::list<User> users;
public:
    static constexpr UUID uuid = 68;
    virtual UUID getEventUuid() const override;

    EventHackUserlistReceived(size_t userId,
                             size_t serverId,
                             const std::string& channel);
    void addUser(const std::string& nick);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    const std::list<User>& getUsers() const;
};

#endif
