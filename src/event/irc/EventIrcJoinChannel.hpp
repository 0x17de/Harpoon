#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "IIrcCommand.hpp"
#include <list>
#include <string>


struct IrcChannelJoinData {
    explicit IrcChannelJoinData(const std::string& name);
    explicit IrcChannelJoinData(const std::string& name, const std::string& password);

    std::string name;
    bool passwordSpecified;
    std::string password;
};

class IrcChannelLoginData;
class EventIrcJoinChannel : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::list<IrcChannelJoinData> loginData;
public:
    static constexpr UUID uuid = 24;
    virtual UUID getEventUuid() const override;

    explicit EventIrcJoinChannel(size_t userId, size_t serverId);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    void addLoginData(const std::string& name, const std::string& password);
    const std::list<IrcChannelJoinData>& getLoginData() const;
};

#endif
