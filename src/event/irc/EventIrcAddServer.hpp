#ifndef EVENTIRCADDSERVER_H
#define EVENTIRCADDSERVER_H

#include "../IUserEvent.hpp"
#include <string>


class IrcChannelLoginData;
class EventIrcAddServer : public IUserEvent {
    size_t userId;
    std::string serverName;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcAddServer(size_t userId, std::string serverName);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getServerName() const;
};

#endif
