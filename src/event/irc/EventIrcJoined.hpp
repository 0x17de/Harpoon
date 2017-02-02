#ifndef EVENTIRCJOINED_H
#define EVENTIRCJOINED_H

#include "../IClientEvent.hpp"
#include "IrcLoggable.hpp"
#include <string>


class EventIrcJoined : public IClientEvent, public IrcLoggable {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
public:
    static constexpr UUID uuid = 25;
    virtual UUID getEventUuid() const override;

    EventIrcJoined(size_t userId, size_t serverId, const std::string& username, const std::string& channel);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
};

#endif
