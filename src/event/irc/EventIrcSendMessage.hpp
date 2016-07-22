#ifndef EVENTIRCSENDMESSAGE_H
#define EVENTIRCSENDMESSAGE_H

#include "IIrcCommand.hpp"
#include <string>


class EventIrcSendMessage : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::string channel;
    std::string message;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcSendMessage(size_t userId, size_t serverId, const std::string& channel, const std::string& message);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getChannel() const;
    std::string getMessage() const;
};

#endif
