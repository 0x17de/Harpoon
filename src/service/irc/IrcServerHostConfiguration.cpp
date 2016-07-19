#include "IrcServerHostConfiguration.hpp"


IrcServerHostConfiguration::IrcServerHostConfiguration() {
}

IrcServerHostConfiguration::IrcServerHostConfiguration(const std::string& hostName,
													   int port,
													   const std::string& password,
													   bool ipV6,
													   bool ssl)
	: hostName{hostName}
	, port{port}
	, password{password}
	, ipV6{ipV6}
	, ssl{ssl}
{
}

std::string IrcServerHostConfiguration::getHostName() const {
	return hostName;
}

int IrcServerHostConfiguration::getPort() const {
	return port;
}

std::string IrcServerHostConfiguration::getPassword() const {
	return password;
}

bool IrcServerHostConfiguration::getIpV6() const {
	return ipV6;
}

bool IrcServerHostConfiguration::getSsl() const {
	return ssl;
}
