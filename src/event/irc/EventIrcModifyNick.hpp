#ifndef EVENTIRCMODIFYNICK_H
#define EVENTIRCMODIFYNICK_H

#include "../IUserEvent.hpp"
#include <string>


class EventIrcModifyNick : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::string oldNick, newNick;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcModifyNick(size_t userId,
                       size_t serverId,
                       const std::string& oldNick,
                       const std::string& newNick);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getOldNick() const;
    std::string getNewNick() const;
};

#endif
