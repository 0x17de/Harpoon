#ifndef EVENTHACKDELETECHANNEL_H
#define EVENTHACKDELETECHANNEL_H

#include "IHackCommand.hpp"
#include <string>


class EventHackDeleteChannel : public IHackCommand {
    size_t userId;
    size_t serverId;
    std::string channelName;
public:
    static constexpr UUID uuid = 56;
    virtual UUID getEventUuid() const override;

    EventHackDeleteChannel(size_t userId,
                          size_t serverId,
                          const std::string& channelName);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getChannelName() const;
};

#endif
