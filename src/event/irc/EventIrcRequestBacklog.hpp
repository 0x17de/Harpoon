#ifndef EVENTIRCREQUESTBACKLOG_H
#define EVENTIRCREQUESTBACKLOG_H

#include "IIrcCommand.hpp"
#include <string>


class EventIrcRequestBacklog : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::string channelName;
    size_t fromId;
    int count;
public:
    static constexpr UUID uuid = 37;
    virtual UUID getEventUuid() const override;

    EventIrcRequestBacklog(size_t userId,
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
