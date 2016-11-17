#ifndef EVENTIRCCHANNELUSER_H
#define EVENTIRCCHANNELUSER_H

#include <string>

class IrcChannelUser {
    std::string nick;
    std::string mode;
public:
    IrcChannelUser(const std::string& nick, const std::string& mode);

    std::string getNick() const;
    std::string getMode() const;
};

#endif
