#ifndef EVENTIRCREQUESTBACKLOG_H
#define EVENTIRCREQUESTBACKLOG_H

#include "IIrcCommand.hpp"
#include <string>


class EventIrcRequestBacklog : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::string channelName;
    size_t fromId;
    size_t count;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcRequestBacklog(size_t userId,
                           size_t serverId,
                           const std::string& channelName,
                           size_t fromId,
                           size_t count);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getChannelName() const;
    size_t getFromId() const;
    size_t getCount() const;
};

#endif
