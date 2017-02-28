#include "HackChannelLoginData.hpp"


HackChannelLoginData::HackChannelLoginData(size_t channelId,
                                         const std::string& channelName,
                                         bool disabled)
    : channelId{channelId}
    , channelName{channelName}
    , disabled{disabled}
{
}

size_t HackChannelLoginData::getChannelId() const {
    return channelId;
}

std::string HackChannelLoginData::getChannelName() const {
    return channelName;
}

bool HackChannelLoginData::getDisabled() const {
    return disabled;
}

void HackChannelLoginData::setDisabled(bool ldisabled) {
    disabled = ldisabled;
}
