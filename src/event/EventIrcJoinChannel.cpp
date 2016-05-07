#include "EventIrcJoinChannel.hpp"
#include "utils/uuidGen.hpp"


EventIrcJoinChannel::EventIrcJoinChannel(size_t serverId, std::list<IrcChannelLoginData> loginData)
:
	serverId{serverId},
	loginData{loginData.begin(), loginData.end()}
{
}

UUID EventIrcJoinChannel::uuid = ::uuid.get();
UUID EventIrcJoinChannel::getEventUuid() const {
	return this->uuid;
}

size_t EventIrcJoinChannel::getServerId() const {
	return serverId;
};
const std::list<IrcChannelLoginData> EventIrcJoinChannel::getLoginData() const {
	return loginData;
};

