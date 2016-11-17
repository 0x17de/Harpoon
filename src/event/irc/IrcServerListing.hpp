#ifndef IRCSERVERLISTING_H
#define IRCSERVERLISTING_H

#include <string>
#include <list>

#include "IrcChannelListing.hpp"

class IrcServerListing {
    std::string activeNick;
    size_t serverId;
    std::string serverName;
    std::list<IrcChannelListing> channels;
public:
    IrcServerListing(std::string activeNick,
                     size_t serverId,
                     const std::string& serverName);
    IrcChannelListing& addChannel(const std::string& channelName,
                                  const std::string& channelTopic,
                                  bool disabled);

    std::string getActiveNick() const;
    size_t getServerId() const;
    std::string getServerName() const;
    const std::list<IrcChannelListing>& getChannels() const;
};

#endif
