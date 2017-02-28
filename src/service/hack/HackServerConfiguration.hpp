#ifndef HACKSERVERCONFIGURATION_H
#define HACKSERVERCONFIGURATION_H

#include "HackChannelLoginData.hpp"
#include "HackServerHostConfiguration.hpp"
#include <list>
#include <string>


class HackServerConfiguration {
    size_t serverId;
    std::string serverName;
    std::list<std::string> nicks;
    std::list<HackChannelLoginData> channels;
    std::list<HackServerHostConfiguration> hostConfigurations;
public:
    HackServerConfiguration(size_t serverId,
                            const std::string& serverName);

    void addNick(const std::string& nick);
    void modifyNick(const std::string& oldNick, const std::string& newNick);
    const std::list<std::string>& getNicks() const;
    void addChannelLoginData(size_t channelId,
                             const std::string& channelName,
                             bool channelDisabled);
    const std::list<HackChannelLoginData>& getChannelLoginData() const;
    size_t getServerId() const;
    std::string getServerName() const;
    void addHostConfiguration(const std::string& hostName,
                              const std::string& websocketUri,
                              int port,
                              bool ipV6,
                              bool ssl);
    void removeHost(const std::string& hostName, int port);
    const std::list<HackServerHostConfiguration>& getHostConfigurations() const;
};

#endif
