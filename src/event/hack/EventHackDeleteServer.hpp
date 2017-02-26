#ifndef EVENTHACKDELETESERVER_H
#define EVENTHACKDELETESERVER_H

#include "../IUserEvent.hpp"


class EventHackDeleteServer : public IUserEvent {
    size_t userId;
    size_t serverId;
public:
    static constexpr UUID uuid = 58;
    virtual UUID getEventUuid() const override;

    EventHackDeleteServer(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
};

#endif
