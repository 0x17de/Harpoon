#include "HackChannelUser.hpp"

HackChannelUser::HackChannelUser(const std::string& nick,
                               const std::string& trip)
    : nick{nick}
    , trip{trip}
{
}

std::string HackChannelUser::getNick() const {
    return nick;
}

std::string HackChannelUser::getTrip() const {
    return trip;
}
