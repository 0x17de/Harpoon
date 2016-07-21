#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "../IUserEvent.hpp"
#include <list>
#include <string>


struct IrcChannelJoinData {
    IrcChannelJoinData(const std::string& name, const std::string& password);

    std::string name;
    std::string password;
};

class IrcChannelLoginData;
class EventIrcJoinChannel : public IUserEvent {
    size_t userId;
    size_t serverId;
    std::list<IrcChannelJoinData> loginData;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcJoinChannel(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    void addLoginData(const std::string& name, const std::string& password);
    const std::list<IrcChannelJoinData>& getLoginData() const;
};

#endif
