#include "EventIrcJoinChannel.h"
#include "utils/uuidGen.h"


EventIrcJoinChannel::EventIrcJoinChannel(std::list<IrcChannelLoginData> loginData)
:
	loginData{loginData.begin(), loginData.end()}
{
}

UUID EventIrcJoinChannel::uuid = ::uuid.get();
UUID EventIrcJoinChannel::getEventUuid() const {
	return this->uuid;
}

