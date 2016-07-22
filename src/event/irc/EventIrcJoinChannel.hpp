#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "IIrcCommand.hpp"
#include <list>
#include <string>


struct IrcChannelJoinData {
    IrcChannelJoinData(const std::string& name, const std::string& password);

    std::string name;
    std::string password;
};

class IrcChannelLoginData;
class EventIrcJoinChannel : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::list<IrcChannelJoinData> loginData;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcJoinChannel(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    void addLoginData(const std::string& name, const std::string& password);
    const std::list<IrcChannelJoinData>& getLoginData() const;
};

#endif
