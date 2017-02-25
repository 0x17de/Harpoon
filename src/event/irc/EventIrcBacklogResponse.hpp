#ifndef EVENTIRCBACKLOGRESPONSE_H
#define EVENTIRCBACKLOGRESPONSE_H

#include "../IClientEvent.hpp"
#include <string>
#include <iomanip>
#include <list>
#include <memory>

enum class IrcDatabaseMessageType;
struct IrcMessageData {
    size_t messageId;
    std::time_t time;
    std::string message;
    IrcDatabaseMessageType type;
    size_t flags;
    std::string sender;

    inline IrcMessageData(size_t messageId,
                          const std::time_t& time,
                          const std::string& message,
                          IrcDatabaseMessageType type,
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
    std::list<IrcMessageData> events;

public:
    static constexpr UUID uuid = 14;
    virtual UUID getEventUuid() const override;

    EventIrcBacklogResponse(size_t userId,
                            size_t serverId,
                            const std::string& channel,
                            std::list<IrcMessageData>&& data);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    const std::list<IrcMessageData>& getData() const;
};

#endif
