#include "HackServerHostConfiguration.hpp"


HackServerHostConfiguration::HackServerHostConfiguration()
    : websocketUri_{"/chat-ws"}
	, port_{0}
	, ipV6_{false}
	, ssl_{true}
{
}

HackServerHostConfiguration::HackServerHostConfiguration(const std::string& hostName,
                                                         const std::string& websocketUri,
                                                         int port,
                                                         const std::string& password,
                                                         bool ipV6,
                                                         bool ssl)
	: hostName_{hostName}
    , websocketUri_{websocketUri}
	, port_{port}
    , password_{password}
	, ipV6_{ipV6}
	, ssl_{ssl}
{
}

std::string HackServerHostConfiguration::getHostName() const {
	return hostName_;
}

std::string HackServerHostConfiguration::getWebsocketUri() const {
	return websocketUri_;
}

int HackServerHostConfiguration::getPort() const {
	return port_;
}

std::string HackServerHostConfiguration::getPassword() const {
    return password_;
}

bool HackServerHostConfiguration::getIpV6() const {
	return ipV6_;
}

bool HackServerHostConfiguration::getSsl() const {
	return ssl_;
}
