#ifndef IRCSERVERCONFIGURATION_H
#define IRCSERVERCONFIGURATION_H

#include "IrcChannelLoginData.hpp"
#include "IrcServerHostConfiguration.hpp"
#include <list>
#include <string>


class IrcServerConfiguration {
    size_t serverId;
    std::string serverName;
    std::list<std::string> nicks;
    std::list<IrcChannelLoginData> channels;
    std::list<IrcServerHostConfiguration> hostConfigurations;
public:
    IrcServerConfiguration(size_t serverId,
                           const std::string& serverName);

    void addNick(const std::string& nick);
    const std::list<std::string>& getNicks() const;
    void addChannelLoginData(size_t channelId, const std::string& channelName, const std::string& channelPassword);
    const std::list<IrcChannelLoginData>& getChannelLoginData() const;
    size_t getServerId() const;
    std::string getServerName() const;
    void addHostConfiguration(const std::string& hostName,
                              int port,
                              const std::string& password,
                              bool ipV6,
                              bool ssl);
    const std::list<IrcServerHostConfiguration>& getHostConfigurations() const;
};

#endif
