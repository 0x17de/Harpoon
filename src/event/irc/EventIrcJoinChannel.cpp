#include "EventIrcJoinChannel.hpp"
#include "utils/uuidGen.hpp"


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

EventIrcJoinChannel::EventIrcJoinChannel(size_t userId, size_t serverId)
:
	userId{userId},
	serverId{serverId}
{
}

UUID EventIrcJoinChannel::uuid = ::uuid.get();
UUID EventIrcJoinChannel::getEventUuid() const {
	return this->uuid;
}

size_t EventIrcJoinChannel::getUserId() const {
	return userId;
}

size_t EventIrcJoinChannel::getServerId() const {
	return serverId;
}
std::list<IrcChannelLoginData> EventIrcJoinChannel::getLoginData() {
	return loginData;
}
const std::list<IrcChannelLoginData> EventIrcJoinChannel::getLoginData() const {
	return loginData;
}

