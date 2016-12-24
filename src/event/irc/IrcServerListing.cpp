#include "IrcServerListing.hpp"

IrcServerListing::IrcServerListing(const std::string& activeNick,
                                   size_t serverId,
                                   const std::string& serverName)
    : activeNick{activeNick}
    , serverId{serverId}
    , serverName{serverName}
{
}

IrcChannelListing& IrcServerListing::addChannel(const std::string& channelName,
                                                const std::string& channelTopic,
                                                bool disabled) {
    channels.emplace_back(channelName,
                          channelTopic,
                          disabled);
    return channels.back();
}

std::string IrcServerListing::getActiveNick() const {
    return activeNick;
}

size_t IrcServerListing::getServerId() const {
    return serverId;
}

std::string IrcServerListing::getServerName() const {
    return serverName;
}

const std::list<IrcChannelListing>& IrcServerListing::getChannels() const {
    return channels;
}
