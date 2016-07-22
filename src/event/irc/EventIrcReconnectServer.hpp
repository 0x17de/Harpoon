#ifndef EVENTIRCRECONNECTSERVER_H
#define EVENTIRCRECONNECTSERVER_H

#include "IIrcCommand.hpp"
#include <cstddef>


class EventIrcReconnectServer : public IIrcCommand {
    size_t userId;
    size_t serverId;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcReconnectServer(size_t userId,
                            size_t serverId);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
};

#endif
