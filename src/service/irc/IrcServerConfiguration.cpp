#include "IrcChannelLoginData.hpp"
#include "IrcServerHostConfiguration.hpp"
#include "IrcServerConfiguration.hpp"
#include <algorithm>

using namespace std;


IrcServerConfiguration::IrcServerConfiguration(size_t serverId,
											   const std::string& serverName)
    : serverId{serverId}
    , serverName{serverName}
{
}

void IrcServerConfiguration::addNick(const std::string& nick) {
    nicks.emplace_back(nick);
}

void IrcServerConfiguration::modifyNick(const std::string& oldNick, const std::string& newNick) {
    if (oldNick.size() == 0) {
        nicks.push_back(newNick);
    } else if (newNick.size() == 0) {
        nicks.remove(oldNick);
    } else {
        auto it = find(nicks.begin(), nicks.end(), oldNick);
        *it = newNick;
    }
}

const std::list<std::string>& IrcServerConfiguration::getNicks() const {
    return nicks;
}

void IrcServerConfiguration::addChannelLoginData(size_t channelId, const std::string& channelName, const std::string& channelPassword) {
    channels.emplace_back(channelId, channelName, channelPassword);
}

const std::list<IrcChannelLoginData>& IrcServerConfiguration::getChannelLoginData() const {
    return channels;
}

size_t IrcServerConfiguration::getServerId() const {
	return serverId;
}

std::string IrcServerConfiguration::getServerName() const {
	return serverName;
}

void IrcServerConfiguration::addHostConfiguration(const std::string& hostName,
												  int port,
												  const std::string& password,
												  bool ipV6,
												  bool ssl) {
	hostConfigurations.emplace_back(hostName, port, password, ipV6, ssl);
}

void IrcServerConfiguration::removeHost(const std::string& hostName, int port) {
    for (auto it = hostConfigurations.begin(); it != hostConfigurations.end(); ++it) {
        auto& host = *it;
        if (host.getHostName() == hostName && host.getPort() == port) {
            hostConfigurations.erase(it);
            break;
        }
    }
}

const std::list<IrcServerHostConfiguration>& IrcServerConfiguration::getHostConfigurations() const {
	return hostConfigurations;
}
