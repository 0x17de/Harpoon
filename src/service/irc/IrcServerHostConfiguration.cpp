#include "IrcServerHostConfiguration.hpp"


IrcServerHostConfiguration::IrcServerHostConfiguration()
	: port_{0}
	, ipV6_{false}
	, ssl_{true}
{
}

IrcServerHostConfiguration::IrcServerHostConfiguration(const std::string& hostName,
													   int port,
													   const std::string& password,
													   bool ipV6,
													   bool ssl)
	: hostName_{hostName}
	, port_{port}
	, password_{password}
	, ipV6_{ipV6}
	, ssl_{ssl}
{
}

std::string IrcServerHostConfiguration::getHostName() const {
	return hostName_;
}

int IrcServerHostConfiguration::getPort() const {
	return port_;
}

std::string IrcServerHostConfiguration::getPassword() const {
	return password_;
}

bool IrcServerHostConfiguration::getIpV6() const {
	return ipV6_;
}

bool IrcServerHostConfiguration::getSsl() const {
	return ssl_;
}
