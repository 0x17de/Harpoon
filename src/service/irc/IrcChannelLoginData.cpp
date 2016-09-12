#include "IrcChannelLoginData.hpp"


IrcChannelLoginData::IrcChannelLoginData(size_t channelId,
                                         const std::string& channelName,
                                         const std::string& channelPassword,
                                         bool disabled)
    : channelId{channelId}
    , channelName{channelName}
    , channelPassword{channelPassword}
    , disabled{disabled}
{
}

size_t IrcChannelLoginData::getChannelId() const {
    return channelId;
}

std::string IrcChannelLoginData::getChannelName() const {
    return channelName;
}

std::string IrcChannelLoginData::getChannelPassword() const {
    return channelPassword;
}

bool IrcChannelLoginData::getDisabled() const {
    return disabled;
}

void IrcChannelLoginData::setDisabled(bool ldisabled) {
    disabled = ldisabled;
}
