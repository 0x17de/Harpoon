#ifndef EVENTHACKSENDMESSAGE_H
#define EVENTHACKSENDMESSAGE_H

#include "EventHackMessageType.hpp"
#include "IHackCommand.hpp"
#include <string>


class EventHackSendMessage : public IHackCommand {
    size_t userId;
    size_t serverId;
    std::string channel;
    std::string message;
public:
    static constexpr UUID uuid = 64;
    virtual UUID getEventUuid() const override;

    EventHackSendMessage(size_t userId,
                         size_t serverId,
                         const std::string& channel,
                         const std::string& message);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getChannel() const;
    std::string getMessage() const;
};

#endif
