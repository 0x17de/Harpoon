#include "HackServerListing.hpp"

HackServerListing::HackServerListing(const std::string& activeNick,
                                     size_t serverId,
                                     const std::string& serverName)
    : activeNick{activeNick}
    , serverId{serverId}
    , serverName{serverName}
{
}

HackChannelListing& HackServerListing::addChannel(const std::string& channelName,
                                                  bool disabled) {
    channels.emplace_back(channelName,
                          disabled);
    return channels.back();
}

std::string HackServerListing::getActiveNick() const {
    return activeNick;
}

size_t HackServerListing::getServerId() const {
    return serverId;
}

std::string HackServerListing::getServerName() const {
    return serverName;
}

const std::list<HackChannelListing>& HackServerListing::getChannels() const {
    return channels;
}
