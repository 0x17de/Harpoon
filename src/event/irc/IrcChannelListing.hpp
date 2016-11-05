#ifndef IRCCHANNELLISTING_H
#define IRCCHANNELLISTING_H

#include <list>
#include <string>

#include "IrcChannelUser.cpp"

class IrcChannelListing {
    std::string channelName;
    std::string channelTopic;
    std::list<IrcChannelUser> users;
    bool disabled;
public:
    IrcChannelListing(const std::string& channelName,
                      const std::string& channelTopic,
                      bool disabled);

    void addUser(const std::string& nick, const std::string& mode);
    const std::list<IrcChannelUser>& getUsers() const;
    std::string getChannelName() const;
    std::string getChannelTopic() const;
    bool getDisabled() const;
};

#endif
