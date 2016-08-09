#ifndef EVENTIRCHOSTADDED_H
#define EVENTIRCHOSTADDED_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcHostAdded : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string host;
    int port;
    std::string password;
    bool ipV6;
    bool ssl;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcHostAdded(size_t userId,
                      size_t serverId,
                      const std::string& host,
                      int port,
                      const std::string& password,
                      bool ipV6,
                      bool ssl);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getHost() const;
    std::string getPassword() const;
    int getPort() const;
    bool getIpV6() const;
    bool getSsl() const;
};

#endif
