#include "EventIrcPartChannel.hpp"
#include "utils/uuidGen.hpp"


EventIrcPartChannel::EventIrcPartChannel(size_t serverId, std::list<size_t> loginData)
:
	serverId{serverId},
	channelIds{channelIds}
{
}

UUID EventIrcPartChannel::uuid = ::uuid.get();
UUID EventIrcPartChannel::getEventUuid() const {
	return this->uuid;
}

size_t EventIrcPartChannel::getServerId() const {
	return serverId;
};
const std::list<size_t> EventIrcPartChannel::getChannelIds() const {
	return channelIds;
};

