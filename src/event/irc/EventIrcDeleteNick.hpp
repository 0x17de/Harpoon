#ifndef EVENTIRCDELETENICK_H
#define EVENTIRCDELETENICK_H

#include "../IUserEvent.hpp"
#include <string>


class EventIrcDeleteNick : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::string nick;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcDeleteNick(size_t userId, size_t serverId, const std::string& nick);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getNick() const;
};

#endif
