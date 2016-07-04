#ifndef EVENTIRCDELETESERVER_H
#define EVENTIRCDELETESERVER_H

#include "../IUserEvent.hpp"


class IrcChannelLoginData;
class EventIrcDeleteServer : public IUserEvent {
    size_t userId;
    size_t serverId;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcDeleteServer(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
};

#endif
