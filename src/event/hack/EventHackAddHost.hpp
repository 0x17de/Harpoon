#ifndef EVENTHACKADDHOST_H
#define EVENTHACKADDHOST_H

#include "../IUserEvent.hpp"
#include <string>


class EventHackAddHost : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::string host;
    std::string websocketUri;
    int port;
    std::string password;
    bool ipV6;
    bool ssl;
public:
    static constexpr UUID uuid = 53;
    virtual UUID getEventUuid() const override;

    EventHackAddHost(size_t userId,
                     size_t serverId,
                     const std::string& host,
                     const std::string& websocketUri,
                     int port,
                     const std::string& password,
                     bool ipV6,
                     bool ssl);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getHost() const;
    std::string getWebsocketUri() const;
    int getPort() const;
    std::string getPassword() const;
    bool getIpV6() const;
    bool getSsl() const;
};

#endif
