#ifndef EVENTHACKHOSTADDED_H
#define EVENTHACKHOSTADDED_H

#include "../IClientEvent.hpp"
#include <string>


class EventHackHostAdded : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string host;
    std::string websocketUri;
    int port;
    bool ipV6;
    bool ssl;
public:
    static constexpr UUID uuid = 69;
    virtual UUID getEventUuid() const override;

    EventHackHostAdded(size_t userId,
                       size_t serverId,
                       const std::string& host,
                       const std::string& websocketUri,
                       int port,
                       bool ipV6,
                       bool ssl);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getHost() const;
    std::string getWebsocketUri() const;
    int getPort() const;
    bool getIpV6() const;
    bool getSsl() const;
};

#endif
