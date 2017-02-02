#ifndef EVENTIRCMESSAGE_H
#define EVENTIRCMESSAGE_H

#include "../IClientEvent.hpp"
#include "EventIrcMessageType.hpp"
#include "IrcLoggable.hpp"
#include <string>


class EventIrcMessage : public IClientEvent, public IrcLoggable {
    size_t userId;
    size_t serverId;
    std::string from;
    std::string channel;
    std::string message;
    MessageType type;
public:
    static constexpr UUID uuid = 27;
    virtual UUID getEventUuid() const override;

    EventIrcMessage(size_t userId,
                    size_t serverId,
                    const std::string& from,
                    const std::string& channel,
                    const std::string& message,
                    MessageType type);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getFrom() const;
    std::string getChannel() const;
    std::string getMessage() const;
    MessageType getType() const;
};

#endif
