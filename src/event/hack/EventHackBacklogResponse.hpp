#ifndef EVENTHACKBACKLOGRESPONSE_H
#define EVENTHACKBACKLOGRESPONSE_H

#include "../IClientEvent.hpp"
#include <string>
#include <iomanip>
#include <list>
#include <memory>

enum class HackDatabaseMessageType;
struct HackMessageData {
    size_t messageId;
    std::time_t time;
    std::string message;
    HackDatabaseMessageType type;
    size_t flags;
    std::string sender;

    inline HackMessageData(size_t messageId,
                           const std::time_t& time,
                           const std::string& message,
                           HackDatabaseMessageType type,
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

class EventHackBacklogResponse : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string channel;
    std::list<HackMessageData> events;

public:
    static constexpr UUID uuid = 55;
    virtual UUID getEventUuid() const override;

    EventHackBacklogResponse(size_t userId,
                             size_t serverId,
                             const std::string& channel,
                             std::list<HackMessageData>&& data);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    const std::list<HackMessageData>& getData() const;
};

#endif
