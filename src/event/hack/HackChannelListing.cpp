#include "HackChannelListing.hpp"

HackChannelListing::HackChannelListing(const std::string& channelName,
                                       bool disabled)
    : channelName{channelName}
    , disabled{disabled}
{
}

void HackChannelListing::addUser(const std::string& nick, const std::string& trip) {
    users.emplace_back(nick, trip);
}

const std::list<HackChannelUser>& HackChannelListing::getUsers() const {
    return users;
}


std::string HackChannelListing::getChannelName() const {
    return channelName;
}

bool HackChannelListing::getDisabled() const {
    return disabled;
}

