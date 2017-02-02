#ifndef EVENTIRCUSERLISTRECEIVED_H
#define EVENTIRCUSERLISTRECEIVED_H

#include "../IClientEvent.hpp"
#include <string>
#include <list>


class EventIrcUserlistReceived : public IClientEvent {
    struct User {
        User(const std::string& nick,
             const std::string& mode);
        std::string nick;
        std::string mode;
    };

    size_t userId;
    size_t serverId;
    std::string channel;
    std::list<User> users;
public:
    static constexpr UUID uuid = 45;
    virtual UUID getEventUuid() const override;

    EventIrcUserlistReceived(size_t userId,
                             size_t serverId,
                             const std::string& channel);
    void addUser(const std::string& nick, const std::string& mode);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    const std::list<User>& getUsers() const;
};

#endif
