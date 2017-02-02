#ifndef EVENTIRCDELETESERVER_H
#define EVENTIRCDELETESERVER_H

#include "../IUserEvent.hpp"


class EventIrcDeleteServer : public IUserEvent {
    size_t userId;
    size_t serverId;
public:
    static constexpr UUID uuid = 20;
    virtual UUID getEventUuid() const override;

    EventIrcDeleteServer(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
};

#endif
