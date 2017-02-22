#ifndef EVENTHACKMESSAGE_H
#define EVENTHACKMESSAGE_H

#include "../IClientEvent.hpp"
#include "EventHackMessageType.hpp"
#include "HackLoggable.hpp"
#include <string>


class EventHackMessage : public IClientEvent, public HackLoggable {
    size_t userId;
    size_t serverId;
    std::string from;
    std::string channel;
    std::string message;
    HackMessageType type;
public:
    static constexpr UUID uuid = 52;
    virtual UUID getEventUuid() const override;

    EventHackMessage(size_t userId,
                     size_t serverId,
                     const std::string& from,
                     const std::string& channel,
                     const std::string& message,
                     HackMessageType type);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getFrom() const;
    std::string getChannel() const;
    std::string getMessage() const;
    HackMessageType getType() const;
};

#endif
