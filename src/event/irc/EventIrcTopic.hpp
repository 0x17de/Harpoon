#ifndef EVENTIRCTOPIC_H
#define EVENTIRCTOPIC_H

#include "../IEvent.hpp"
#include <string>


class EventIrcTopic : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string channel;
	std::string topic;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcTopic(size_t userId, size_t serverId, const std::string& username, const std::string& channel, const std::string& topic);
	size_t getUserId() const;
	size_t getServerId() const;
};

#endif
