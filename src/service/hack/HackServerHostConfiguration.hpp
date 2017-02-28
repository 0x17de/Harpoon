#ifndef HACKSERVERHOSTCONFIGURATION_H
#define HACKSERVERHOSTCONFIGURATION_H

#include <string>


class HackServerHostConfiguration {
    std::string hostName_;
    std::string websocketUri_;
    int port_;
    bool ipV6_;
    bool ssl_;
public:
    HackServerHostConfiguration();
    HackServerHostConfiguration(const std::string& hostName,
                                const std::string& websocketUri,
                                int port,
                                bool ipV6,
                                bool ssl);

    std::string getHostName() const;
    std::string getWebsocketUri() const;
    int getPort() const;
    bool getIpV6() const;
    bool getSsl() const;
};

#endif
