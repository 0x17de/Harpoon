#include "EventIrcJoinChannel.hpp"
#include "utils/uuidGen.hpp"


IrcChannelLoginData::IrcChannelLoginData(size_t channelId, const std::string& channel, const std::string& password)
:
	channelId{channelId},
	channel{channel},
	password{password}
{
}

EventIrcJoinChannel::EventIrcJoinChannel(size_t userId, size_t serverId, std::list<IrcChannelLoginData> loginData)
:
	serverId{serverId},
	loginData{loginData.begin(), loginData.end()}
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
const std::list<IrcChannelLoginData> EventIrcJoinChannel::getLoginData() const {
	return loginData;
}

