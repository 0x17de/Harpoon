#ifndef HACKSERVERLISTING_H
#define HACKSERVERLISTING_H

#include <string>
#include <list>

#include "HackChannelListing.hpp"

class HackServerListing {
    std::string activeNick;
    size_t serverId;
    std::string serverName;
    std::list<HackChannelListing> channels;
public:
    HackServerListing(const std::string& activeNick,
                      size_t serverId,
                      const std::string& serverName);
    HackChannelListing& addChannel(const std::string& channelName,
                                   bool disabled);

    std::string getActiveNick() const;
    size_t getServerId() const;
    std::string getServerName() const;
    const std::list<HackChannelListing>& getChannels() const;
};

#endif
