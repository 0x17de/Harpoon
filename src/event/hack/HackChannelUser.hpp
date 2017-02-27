#ifndef EVENTHACKCHANNELUSER_H
#define EVENTHACKCHANNELUSER_H

#include <string>

class HackChannelUser {
    std::string nick;
    std::string trip;
public:
    HackChannelUser(const std::string& nick, const std::string& trip);

    std::string getNick() const;
    std::string getTrip() const;
};

#endif
