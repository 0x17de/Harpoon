#ifndef EVENTHACKREQUESTBACKLOG_H
#define EVENTHACKREQUESTBACKLOG_H

#include "IHackCommand.hpp"
#include <string>


class EventHackRequestBacklog : public IHackCommand {
    size_t userId;
    size_t serverId;
    std::string channelName;
    size_t fromId;
    int count;
public:
    static constexpr UUID uuid = 63;
    virtual UUID getEventUuid() const override;

    EventHackRequestBacklog(size_t userId,
                            size_t serverId,
                            const std::string& channelName,
                            size_t fromId,
                            int count);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getChannelName() const;
    size_t getFromId() const;
    int getCount() const;
};

#endif
