#ifndef EVENTHACKNICKMODIFIED_H
#define EVENTHACKNICKMODIFIED_H

#include "../IClientEvent.hpp"
#include <string>


class EventHackNickModified : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string oldNick;
    std::string newNick;
public:
    static constexpr UUID uuid = 71;
    virtual UUID getEventUuid() const override;

    EventHackNickModified(size_t userId, size_t serverId, const std::string& oldNick, const std::string& newNick);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getOldNick() const;
    std::string getNewNick() const;
};

#endif
