#ifndef IRCSERVERHOSTCONFIGURATION_H
#define IRCSERVERHOSTCONFIGURATION_H

#include <string>


class IrcServerHostConfiguration {
    std::string hostName;
    int port;
    std::string password;
    bool ipV6;
    bool ssl;
public:
    IrcServerHostConfiguration(const std::string& hostName,
                               int port,
                               const std::string& password,
                               bool ipV6,
                               bool ssl);

    std::string getHostName() const;
    int getPort() const;
    std::string getPassword() const;
    bool getIpV6() const;
    bool getSsl() const;
};

#endif
