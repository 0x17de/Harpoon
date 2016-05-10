#ifndef EVENTIRCPARTED_H
#define EVENTIRCPARTED_H

#include "IEvent.hpp"
#include <string>


class EventIrcParted : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string channel;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcParted(size_t userId, size_t serverId, std::string username, std::string channel);
	size_t getUserId() const;
	size_t getServerId() const;
	std::string getUsername() const;
	std::string getChannel() const;
};

#endif
