#include "IrcChannelListing.hpp"

IrcChannelListing::IrcChannelListing(const std::string& channelName,
                                     const std::string& channelTopic,
                                     bool disabled)
    : channelName{channelName}
    , channelTopic{channelTopic}
    , disabled{disabled}
{
}

void IrcChannelListing::addUser(const std::string& nick, const std::string& mode) {
    users.emplace_back(nick, mode);
}

const std::list<IrcChannelUser>& IrcChannelListing::getUsers() const {
    return users;
}


std::string IrcChannelListing::getChannelName() const {
    return channelName;
}

std::string IrcChannelListing::getChannelTopic() const {
    return channelTopic;
}

bool IrcChannelListing::getDisabled() const {
    return disabled;
}

