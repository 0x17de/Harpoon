#include "HackChannelLoginData.hpp"
#include "HackServerHostConfiguration.hpp"
#include "HackServerConfiguration.hpp"
#include <algorithm>

using namespace std;


HackServerConfiguration::HackServerConfiguration(size_t serverId,
                                                 const std::string& serverName)
    : serverId{serverId}
    , serverName{serverName}
{
}

void HackServerConfiguration::addNick(const std::string& nick) {
    nicks.emplace_back(nick);
}

void HackServerConfiguration::modifyNick(const std::string& oldNick, const std::string& newNick) {
    if (oldNick.size() == 0) {
        nicks.push_back(newNick);
    } else if (newNick.size() == 0) {
        nicks.remove(oldNick);
    } else {
        auto it = find(nicks.begin(), nicks.end(), oldNick);
        if (it == nicks.end()) {
            nicks.push_back(newNick);
        } else {
            *it = newNick;
        }
    }
}

const std::list<std::string>& HackServerConfiguration::getNicks() const {
    return nicks;
}

void HackServerConfiguration::addChannelLoginData(size_t channelId,
                                                  const std::string& channelName,
                                                  bool channelDisabled) {
    channels.emplace_back(channelId,
                          channelName,
                          channelDisabled);
}

const std::list<HackChannelLoginData>& HackServerConfiguration::getChannelLoginData() const {
    return channels;
}

size_t HackServerConfiguration::getServerId() const {
	return serverId;
}

std::string HackServerConfiguration::getServerName() const {
	return serverName;
}

void HackServerConfiguration::addHostConfiguration(const std::string& hostName,
                                                   const std::string& websocketUri,
                                                   int port,
                                                   bool ipV6,
                                                   bool ssl) {
	hostConfigurations.emplace_back(hostName, websocketUri, port, ipV6, ssl);
}

void HackServerConfiguration::removeHost(const std::string& hostName, int port) {
    for (auto it = hostConfigurations.begin(); it != hostConfigurations.end(); ++it) {
        auto& host = *it;
        if (host.getHostName() == hostName && host.getPort() == port) {
            hostConfigurations.erase(it);
            break;
        }
    }
}

const std::list<HackServerHostConfiguration>& HackServerConfiguration::getHostConfigurations() const {
	return hostConfigurations;
}
