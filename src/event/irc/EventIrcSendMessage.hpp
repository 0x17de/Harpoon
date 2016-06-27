#ifndef EVENTIRCSENDMESSAGE_H
#define EVENTIRCSENDMESSAGE_H

#include "../IUserEvent.hpp"
#include <string>


class EventIrcSendMessage : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::string channel;
    std::string message;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcSendMessage(size_t userId, size_t serverId, std::string channel, std::string message);
    size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    std::string getMessage() const;
};

#endif
