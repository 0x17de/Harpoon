#ifndef EVENTIRCQUIT_H
#define EVENTIRCQUIT_H

#include "../IEvent.hpp"
#include "../IClientEvent.hpp"
#include "IrcLoggable.hpp"
#include <cstdlib>
#include <string>


class EventIrcQuit : public IClientEvent, public IrcLoggable {
    size_t userId;
    size_t serverId;
    std::string who;
    std::string reason;
public:
    static constexpr UUID uuid = 35;
    virtual UUID getEventUuid() const override;

    EventIrcQuit(size_t userId, size_t serverId, const std::string& who, const std::string& reason);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getWho() const;
    std::string getReason() const;
};

#endif
