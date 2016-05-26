#ifndef EVENTIRCPARTCHANNEL_H
#define EVENTIRCPARTCHANNEL_H

#include "../IEvent.hpp"
#include <list>
#include <string>


class EventIrcPartChannel : public IEvent {
	size_t serverId;
	std::list<std::string> channels;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcPartChannel(size_t serverId, std::list<std::string> partChannels);
	size_t getServerId() const;
	const std::list<std::string> getChannels() const;
};

#endif
