#ifndef EVENTIRCNICKCHANGED_H
#define EVENTIRCNICKCHANGED_H

#include "../IClientEvent.hpp"
#include "IrcLoggable.hpp"
#include <string>


class EventIrcNickChanged : public IClientEvent, public IrcLoggable {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string newNick;
public:
    static constexpr UUID uuid = 30;
    virtual UUID getEventUuid() const override;

    EventIrcNickChanged(size_t userId, size_t serverId, const std::string& username, const std::string& newNick);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getNewNick() const;
};

#endif
