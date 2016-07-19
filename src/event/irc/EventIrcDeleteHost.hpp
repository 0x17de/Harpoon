#ifndef EVENTIRCDELETEHOST_H
#define EVENTIRCDELETEHOST_H

#include "../IUserEvent.hpp"
#include <string>


class EventIrcDeleteHost : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::string host;
    int port;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcDeleteHost(size_t userId,
                       size_t serverId,
                       const std::string& host,
                       int port);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getHost() const;
    int getPort() const;
};

#endif
