#ifndef EVENTIRCMODIFYNICK_H
#define EVENTIRCMODIFYNICK_H

#include "IIrcCommand.hpp"
#include <string>


class EventIrcModifyNick : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::string oldNick, newNick;
public:
    static constexpr UUID uuid = 29;
    virtual UUID getEventUuid() const override;

    EventIrcModifyNick(size_t userId,
                       size_t serverId,
                       const std::string& oldNick,
                       const std::string& newNick);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getOldNick() const;
    std::string getNewNick() const;
};

#endif
