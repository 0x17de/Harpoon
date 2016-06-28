#include "IrcChannelLoginData.hpp"


IrcChannelLoginData::IrcChannelLoginData(size_t channelId,
										 const std::string& channelName,
										 const std::string& channelPassword)
	: channelId{channelId}
	, channelName{channelName}
	, channelPassword{channelPassword}
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
