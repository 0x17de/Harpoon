#ifndef EVENTHACKRECONNECTSERVER_H
#define EVENTHACKRECONNECTSERVER_H

#include "IHackCommand.hpp"
#include <cstddef>


class EventHackReconnectServer : public IHackCommand {
    size_t userId;
    size_t serverId;
public:
    static constexpr UUID uuid = 62;
    virtual UUID getEventUuid() const override;

    EventHackReconnectServer(size_t userId,
                             size_t serverId);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
};

#endif
