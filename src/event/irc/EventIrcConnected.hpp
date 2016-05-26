#ifndef EVENTIRCCONNECTED_H
#define EVENTIRCCONNECTED_H

#include "../IEvent.hpp"
#include <cstdlib>


class EventIrcConnected : public IEvent {
	size_t userId;
	size_t serverId;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcConnected(size_t userId, size_t serverId);
	size_t getUserId() const;
	size_t getServerId() const;
};

#endif
