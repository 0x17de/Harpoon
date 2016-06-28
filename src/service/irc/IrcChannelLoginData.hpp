#ifndef IRCCHANNELLOGINDATA_H
#define IRCCHANNELLOGINDATA_H

#include <string>


class IrcChannelLoginData {
    size_t channelId;
    std::string channelName;
    std::string channelPassword;
public:
    IrcChannelLoginData(size_t channelId, const std::string& channel, const std::string& channelPassword);

    size_t getChannelId() const;
    std::string getChannelName() const;
    std::string getChannelPassword() const;
};

#endif
