#ifndef EVENTIRCPARTCHANNEL_H
#define EVENTIRCPARTCHANNEL_H

#include "IEvent.h"
#include <list>
#include <string>


class EventIrcPartChannel : public IEvent {
	size_t serverId;
	std::list<size_t> channelIds;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcPartChannel(size_t serverId, std::list<size_t> channelIds);
	size_t getServerId() const;
	const std::list<size_t> getChannelIds() const;
};

#endif
