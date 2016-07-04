#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "../IUserEvent.hpp"
#include <list>
#include <string>


class IrcChannelLoginData;
class EventIrcJoinChannel : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::list<IrcChannelLoginData> loginData;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcJoinChannel(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::list<IrcChannelLoginData> getLoginData();
    const std::list<IrcChannelLoginData> getLoginData() const;
};

#endif
