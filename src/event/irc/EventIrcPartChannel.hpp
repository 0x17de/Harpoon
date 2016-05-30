#ifndef EVENTIRCPARTCHANNEL_H
#define EVENTIRCPARTCHANNEL_H

#include "../IUserEvent.hpp"
#include <list>
#include <string>


class EventIrcPartChannel : public IUserEvent {
	size_t userId;
	size_t serverId;
	std::list<std::string> channels;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcPartChannel(size_t userId, size_t serverId, std::list<std::string> partChannels);
	virtual size_t getUserId() const override;
	size_t getServerId() const;
	const std::list<std::string> getChannels() const;
};

#endif
