#ifndef EVENTIRCRECONNECTSERVER_H
#define EVENTIRCRECONNECTSERVER_H

#include "../IUserEvent.hpp"
#include <cstddef>


class EventIrcReconnectServer : public IUserEvent {
    size_t userId;
    size_t serverId;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcReconnectServer(size_t userId,
                            size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
};

#endif
