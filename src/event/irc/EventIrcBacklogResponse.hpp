#ifndef EVENTIRCBACKLOGRESPONSE_H
#define EVENTIRCBACKLOGRESPONSE_H

#include "../IClientEvent.hpp"
#include <string>
#include <list>
#include <memory>

enum class MessageType;
struct MessageData {
    size_t messageId;
    std::string time;
    std::string message;
    MessageType type;
    size_t flags;
    std::string sender;

    inline MessageData(size_t messageId,
                       const std::string& time,
                       const std::string& message,
                       MessageType type,
                       size_t flags,
                       const std::string& sender)
        : messageId{messageId}
        , time{time}
        , message{message}
        , type{type}
        , flags{flags}
        , sender{sender}
    { }
};

class EventIrcBacklogResponse : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string channel;
    std::list<MessageData> events;

public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcBacklogResponse(size_t userId,
                            size_t serverId,
                            const std::string& channel,
                            std::list<MessageData>&& data);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    const std::list<MessageData>& getData() const;
};

#endif
