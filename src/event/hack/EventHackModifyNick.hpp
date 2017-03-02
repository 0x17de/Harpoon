#ifndef EVENTHACKMODIFYNICK_H
#define EVENTHACKMODIFYNICK_H

#include "IHackCommand.hpp"
#include <string>


class EventHackModifyNick : public IHackCommand {
    size_t userId;
    size_t serverId;
    std::string oldNick, newNick;
public:
    static constexpr UUID uuid = 70;
    virtual UUID getEventUuid() const override;

    EventHackModifyNick(size_t userId,
                        size_t serverId,
                        const std::string& oldNick,
                        const std::string& newNick);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getOldNick() const;
    std::string getNewNick() const;
};

#endif
