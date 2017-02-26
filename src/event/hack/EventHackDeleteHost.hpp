#ifndef EVENTHACKDELETEHOST_H
#define EVENTHACKDELETEHOST_H

#include "../IUserEvent.hpp"
#include <string>


class EventHackDeleteHost : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::string host;
    int port;
public:
    static constexpr UUID uuid = 57;
    virtual UUID getEventUuid() const override;

    EventHackDeleteHost(size_t userId,
                        size_t serverId,
                        const std::string& host,
                        int port);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getHost() const;
    int getPort() const;
};

#endif
