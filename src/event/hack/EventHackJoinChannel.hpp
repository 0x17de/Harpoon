#ifndef EVENTHACKJOINCHANNEL_H
#define EVENTHACKJOINCHANNEL_H

#include "IHackCommand.hpp"
#include <list>
#include <string>


class HackChannelLoginData;
class EventHackJoinChannel : public IHackCommand {
    size_t userId;
    size_t serverId;
    std::string channel;
public:
    static constexpr UUID uuid = 59;
    virtual UUID getEventUuid() const override;

    explicit EventHackJoinChannel(size_t userId, size_t serverId, const std::string& channel);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getChannel() const;
};

#endif
