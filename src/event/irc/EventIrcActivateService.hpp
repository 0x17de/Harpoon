#ifndef EVENTIRCACTIVATEUSER_H
#define EVENTIRCACTIVATEUSER_H

#include "../IActivateServiceEvent.hpp"
#include <string>
#include <map>
#include <list>

class IrcChannelLoginData;


struct IrcServerConfiguration {
    IrcServerConfiguration(size_t serverId,
                           const std::string& serverName,
                           const std::string& hostname,
                           int port,
                           const std::string& password,
                           bool ipv6,
                           bool ssl);
    void addNick(const std::string& nick);
    const std::list<std::string>& getNicks() const;
    void addChannelLoginData(size_t channelId, const std::string& channelName, const std::string& channelPassword);
    const std::list<IrcChannelLoginData>& getChannelLoginData() const;

    size_t serverId;
    std::string serverName;
    std::string hostname;
    int port;
    std::string password;
    std::list<std::string> nicks;
    std::list<IrcChannelLoginData> channels;
    bool ipv6;
    bool ssl;
};

class EventIrcActivateService : public IActivateServiceEvent {
    size_t userId;
    std::map<size_t, IrcServerConfiguration> loginData;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcActivateService(size_t userId);
    virtual std::shared_ptr<EventLoop> instantiateService(size_t userId, EventQueue* appQueue) const override;

    size_t getUserId() const;
    IrcServerConfiguration& addLoginConfiguration(size_t serverId,
                                                  const std::string& serverName,
                                                  const std::string& hostname,
                                                  int port,
                                                  const std::string& password,
                                                  bool ipv6,
                                                  bool ssl);
    const std::map<size_t, IrcServerConfiguration>& getLoginConfiguration() const;
};

#endif
