#ifndef HACKCHANNELLISTING_H
#define HACKCHANNELLISTING_H

#include <list>
#include <string>

#include "HackChannelUser.hpp"

class HackChannelListing {
    std::string channelName;
    std::list<HackChannelUser> users;
    bool disabled;
public:
    HackChannelListing(const std::string& channelName,
                       bool disabled);

    void addUser(const std::string& nick, const std::string& trip);
    const std::list<HackChannelUser>& getUsers() const;
    std::string getChannelName() const;
    bool getDisabled() const;
};

#endif
