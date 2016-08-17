#ifndef EVENTIRCMESSAGE_H
#define EVENTIRCMESSAGE_H

#include "../IClientEvent.hpp"
#include "IIrcLoggable.hpp"
#include <string>


class EventIrcMessage : public IClientEvent, public IIrcLoggable {
    size_t userId;
    size_t serverId;
    std::string from;
    std::string channel;
    std::string message;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcMessage(size_t userId, size_t serverId, const std::string& from, const std::string& channel, const std::string& message);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getFrom() const;
    std::string getChannel() const;
    std::string getMessage() const;
};

#endif
