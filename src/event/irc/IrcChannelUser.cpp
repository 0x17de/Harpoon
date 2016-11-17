#include "IrcChannelUser.hpp"

IrcChannelUser::IrcChannelUser(const std::string& nick,
                               const std::string& mode)
    : nick{nick}
    , mode{mode}
{
}

std::string IrcChannelUser::getNick() const {
    return nick;
}

std::string IrcChannelUser::getMode() const {
    return mode;
}
