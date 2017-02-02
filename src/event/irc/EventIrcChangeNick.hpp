#ifndef EVENTIRCCHANGENICK_H
#define EVENTIRCCHANGENICK_H

#include "IIrcCommand.hpp"
#include <string>


class EventIrcChangeNick : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::string nick;
public:
    static constexpr UUID uuid = 15;
    virtual UUID getEventUuid() const override;

    EventIrcChangeNick(size_t userId,
                       size_t serverId,
                       const std::string& nick);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getNick() const;
};

#endif
